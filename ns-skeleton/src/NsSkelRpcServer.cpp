/*
 * Copyright 2019 dmerkushov.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * File:   NsSkelRpcServer.cpp
 * Author: dmerkushov
 *
 * Created on March 12, 2019, 1:59 PM
 */

#include "NsSkelRpcServer.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <atomic>
#include <exception>
#include <functional>
#include <thread>
#include <list>
#include <iostream>
#include <sstream>
#include <mutex>
#include <memory>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "NsSkelConfiguration.h"
#include "NsRpcExecutor.h"
#include "NsSerializer.h"
#include "NsSkelRpcRegistry.h"
#include "NsCmdLineParameters.h"

#define BUF_SIZE 500
#define HOST "127.0.0.1"
#define PORT "3425"
#define SLEEP_BETWEEN_ACCEPTS_MICROSEC (10000)

using namespace std;
using namespace nanoservices;

mutex nanoservices::cout_lock;

static thread serverThread;

NsSkelRpcServer::NsSkelRpcServer() :
		_serverActive(false),
		_serverStarted(false),
		_shutdownReceived(false) {
	_host = NsSkelRpcRegistry::instance()->getLocalService()->host();
	_port = NsSkelRpcRegistry::instance()->getLocalService()->port();
	if (NsCmdLineParameters::instance()->isParam("port")) {
		string port_s = NsCmdLineParameters::instance()->paramValue("port");
		_port = stoi(port_s);
	}
}

NsSkelRpcServer::~NsSkelRpcServer() {
}

void NsSkelRpcServer::serverThreadWork() {
	const char *hostname = NULL;

	stringstream portss;
	portss << _port;

	const char *portname = portss.str().c_str();

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_protocol = 0;

	addrinfo *res;

	int err = getaddrinfo(hostname, portname, &hints, &res);
	if (err != 0) {
		throw NsException(NSE_POSITION, "getaddrinfo()");
	}

	_serverSocketFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (_serverSocketFd < 0) {
		throw NsException(NSE_POSITION, "socket()");
	}

	int reuseaddr = 1;
	if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) < 0) {
		throw NsException(NSE_POSITION, "setsockopt() - REUSEADDR");
	}

	int fcntlResult = fcntl(_serverSocketFd, F_SETFL, O_NONBLOCK);
	if (fcntlResult == -1) {
		throw NsException(NSE_POSITION, "fcntl() - O_NONBLOCK");
	}

	if (bind(_serverSocketFd, res->ai_addr, res->ai_addrlen) < 0) {
		throw NsException(NSE_POSITION, "bind()");
	}

	if (listen(_serverSocketFd, 10)) {
		throw NsException(NSE_POSITION, "listen()");
	}

	freeaddrinfo(res);

	//TODO Refactor to using logging engine
	cout_lock.lock();
	cout << "Listening on port " << portname << endl;
	cout_lock.unlock();

	atomic<bool> serverActive(_serverActive.load() && !_shutdownReceived.load());

	//	_serverStateMutex.lock ();
	//	bool serverActive = _serverActive && !_shutdownReceived;
	//	_serverStateMutex.unlock ();

	while (serverActive.load()) {
		sockaddr_storage sa;
		memset(&sa, 0, sizeof(sa));
		socklen_t sl = sizeof(sa);
		int dataSocketFd;
		int e;
		do {
			serverActive = _serverActive.load() && !_shutdownReceived.load();

			dataSocketFd = accept(_serverSocketFd, (sockaddr *) &sa, &sl);
			e = errno;

			if (dataSocketFd < 0 && e == EWOULDBLOCK) {
				::usleep(SLEEP_BETWEEN_ACCEPTS_MICROSEC);
			}
		} while (serverActive.load() && dataSocketFd == -1 && (e == EAGAIN || e == EWOULDBLOCK || e == EINTR));
		if (!serverActive.load()) {
			break;
		}
		if (dataSocketFd < 0) {
			cout_lock.lock();
			cerr << "accept() for the listening socket returned " << dataSocketFd << " (<0), errno=" << errno << "("
				 << (errno == EINTR ? "EINTR" : "not EINTR") << ")" << endl;
			cout_lock.lock();
			continue;
		}

		_threads[dataSocketFd] = make_shared<thread>(
				std::bind(&NsSkelRpcServer::processIncomingConnection, this, dataSocketFd));
		_threads[dataSocketFd]->detach();

		//		cout_lock.lock ();
		//		cout << "Data socket fd=" << dataSocketFd << " is handled by thread " << _threads[dataSocketFd]->get_id () << endl;
		//		cout_lock.unlock ();
	}

	cout_lock.lock();
	cout << "Shutting down the RPC server" << endl;
	cout_lock.unlock();

	close(_serverSocketFd);

	_serverActive = false;
}

void NsSkelRpcServer::startup() throw(NsException) {
	if (_serverStarted) {
		return;
	}

	_serverActive = true;
	_serverStarted = true;

	_serverThread = std::thread(std::bind(&NsSkelRpcServer::serverThreadWork, this));
	_serverThread.detach();
}

void NsSkelRpcServer::shutdown() {
	_shutdownReceived = true;
}

bool NsSkelRpcServer::active() {
	bool active = _serverActive;

	return active;
}

void NsSkelRpcServer::sleepWhileActive() {
	while (_serverActive.load()) {
		::sleep(1);
	}
}

shared_ptr<string> NsSkelRpcServer::host() {
	return _host;
}

uint16_t NsSkelRpcServer::port() {
	return _port;
}

void NsSkelRpcServer::setPort(uint16_t port) {
	_port = port;
}

void NsSkelRpcServer::processIncomingConnection(int dataSocketFd) {
	sockaddr_storage sa;
	memset(&sa, 0, sizeof(sa));
	socklen_t sl;
	memset(&sl, 0, sizeof(sl));

	if (getpeername(dataSocketFd, (sockaddr *) &sa,
					&sl)) { // Need to call getpeername(), since accept() fails to supply valid data
		cout_lock.lock();
		cerr << "processIncomingConnection(): getpeername() failed" << endl;
		cout_lock.unlock();

		NsMonitoring::monitorSendRpcResponseError("processIncomingConnection(): getpeername() failed");
	}

	if (getpeername(dataSocketFd, (sockaddr *) &sa,
					&sl)) { // Need to call getpeername() twice, because it sometimes fails to supply valid data on the first call
		cout_lock.lock();
		cerr << "processIncomingConnection(): getpeername()-2 failed" << endl;
		cout_lock.unlock();

		NsMonitoring::monitorSendRpcResponseError("processIncomingConnection(): getpeername()-2 failed");
	}

	char addrbuf[INET6_ADDRSTRLEN];
	if (getnameinfo((sockaddr *) &sa, sl, addrbuf, sizeof(addrbuf), 0, 0, NI_NUMERICHOST)) {
		cout_lock.lock();
		cerr << "processIncomingConnection(): getnameinfo() failed" << endl;
		cout_lock.unlock();

		NsMonitoring::monitorSendRpcResponseError("processIncomingConnection(): getnameinfo() failed");
	}

	cout_lock.lock();
	cout << "processIncomingConnection(): Server socket " << dataSocketFd << ", client address " << addrbuf << endl;
	cout_lock.unlock();

	//	cout_lock.lock ();
	//	cout << "Server: reading msglen" << endl;
	//	cout_lock.unlock ();

	uint32_t msglen;
	ssize_t msglenrcount;
	msglenrcount = read(dataSocketFd, &msglen, sizeof(msglen));

	if (msglenrcount < ((ssize_t) sizeof(msglen))) {
		cout_lock.lock();
		cerr << "processIncomingConnection(): Cannot read incoming message length (" << sizeof(msglen) << " bytes)"
			 << endl;
		cout_lock.unlock();

		NsMonitoring::monitorSendRpcResponseError("processIncomingConnection(): Cannot read incoming message length");

		return;
	}

	//	cout_lock.lock ();
	//	cout << "Server: incoming message len (network byte order): " << msglen << endl;
	//	cout_lock.unlock ();

	msglen = ntohl(msglen);

	//	cout_lock.lock ();
	//	cout << "Server: incoming message len (host byte order): " << msglen << endl;
	//	cout_lock.unlock ();

	shared_ptr<NsBinBuffer> buf = readBin(dataSocketFd, msglen);

	shared_ptr<NsSerialized> rpcRequestSerialized;
	bool waitForResponse = true;
	shared_ptr<NsSerialized> rpcResultSerialized;
	{
		uint32_t bufSize = buf->size();
		char *bufData = buf->release();

		rpcRequestSerialized = make_shared<NsSerialized>(bufData, bufSize);
		rpcResultSerialized = processRpcRequest(rpcRequestSerialized, waitForResponse);

		::free(bufData);
	}

	if (waitForResponse) {
		uint32_t responseSize = rpcResultSerialized->size;
		uint32_t responseSizeNet = htonl(responseSize);

		writeBin(dataSocketFd, &responseSizeNet, sizeof(responseSizeNet));
		writeBin(dataSocketFd, rpcResultSerialized->ptr, responseSize);

		NsMonitoring::monitorSendRpcResponse(0x807060504030201); // TODO Generate IDs
	}

	if (close(dataSocketFd)) {
		cout_lock.lock();
		cerr << "processIncomingConnection(): close() failed on socket " << dataSocketFd << endl;
		cout_lock.unlock();
	}
}
