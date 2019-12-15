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
 * File:   NsSkelRpcServer.h
 * Author: dmerkushov
 *
 * Created on March 12, 2019, 1:59 PM
 */

#ifndef NSSKELRPCSERVER_H
#define NSSKELRPCSERVER_H

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <map>
#include <exception>
#include <functional>
#include <thread>
#include <list>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
#include <memory>
#include <vector>

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "NsRpcRequest.h"
#include "NsRpcResponse.h"
#include "NsException.h"
#include "NsMonitoring.h"
#include "NsRpcResponseError.h"
#include "NsSerializer.h"
#include "NsSkelRpcService.h"
#include "NsSkelRpcRegistry.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelRpcRegistry.h"
#include "NsRpcRequest.h"
#include "NsRpcResponse.h"
#include "NsRpcResponseError.h"
#include "NsSerializer.h"

namespace nanoservices {

	const int MAX_INTERRUPTED_TIMES=3;

// These serializers are defined in NsRpcExecutor.cpp
	extern NsSerializer<NsRpcRequest> _requestSerializer;
	extern NsSerializer<NsRpcResponseError> _errorSerializer;
	extern NsSerializer<NsRpcResponse> _responseSerializer;

// This mutex is defined in NsSkelRpcServer.cpp
	extern std::mutex cout_lock;

	class NsSkelRpcServer : public std::enable_shared_from_this<NsSkelRpcServer> {
	public:
		NsSkelRpcServer();

		virtual ~NsSkelRpcServer();

		void startup() throw(NsException);

		void shutdown();

		bool active();

		void sleepWhileActive();

		uint16_t port();

	protected:
		void setPort(uint16_t port);

		virtual void processIncomingConnection(int dataSocketFd);

	private:
		NsSkelRpcServer(const NsSkelRpcServer &orig) = delete;

		void operator=(NsSkelRpcServer &orig) = delete;

		uint16_t _port;
		int _serverSocketFd;
		std::thread _serverThread;
		std::atomic<bool> _serverActive;
		std::atomic<bool> _serverStarted;
		std::atomic<bool> _shutdownReceived;
		std::map<int, std::shared_ptr<std::thread> > _threads;

		virtual void serverThreadWork();
	};

	inline std::shared_ptr<NsBinBuffer> readBin(int dataSocketFd, uint32_t len) {
		std::shared_ptr<NsBinBuffer> buf = std::make_shared<NsBinBuffer>(len);
		std::shared_ptr<NsBinBuffer> empty;

		char currByte;
		ssize_t rcount = 0;
		ssize_t totalcount = 0;
		while (totalcount < len) {
			rcount = read(dataSocketFd, &currByte, 1);
			if (rcount == 0) {
				cout_lock.lock();
				std::cerr << "readBin: Socket closed prematurely: fd=" << dataSocketFd << std::endl;
				cout_lock.unlock();
				return empty;
			}
			buf->write(&currByte, 1);
			totalcount++;
		}

		//	cout_lock.lock ();
		//	std::cout << "Read from socket " << dataSocketFd << ":" << std::endl;
		//	std::cout << hexdump (buf->data (), len);
		//	cout_lock.unlock ();

		return buf;
	}

	inline void writeBin(int dataSocketFd, const void *data, size_t len) {
		size_t index = 0;
		const char *d = (char *) data;
		int interruptedTimes = 0;
		while (index < len && interruptedTimes < MAX_INTERRUPTED_TIMES) {
			ssize_t count = write(dataSocketFd, d + index, len - index);
			if (count < 0) {
				if (errno == EINTR) {
					interruptedTimes++;
					continue;
				}
				cout_lock.lock();
				std::cerr << "writeBin: Socket closed prematurely: fd=" << dataSocketFd << std::endl;
				cout_lock.unlock();
			} else {
				index += count;
			}
		}

		//	cout_lock.lock ();
		//	std::cout << "Wrote to socket " << dataSocketFd << ":" << std::endl;
		//	std::cout << hexdump (d, len);
		//	cout_lock.unlock ();
	}

	static std::shared_ptr<NsRpcResponse>
	sendPackRpcRequest(std::shared_ptr<std::string> serviceName, const char *host, uint16_t port,
					   std::shared_ptr<NsRpcRequest> request) throw(NsException) {

		char portCharPtr[50];
		sprintf(portCharPtr, "%d", port);

		addrinfo hints;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
		hints.ai_socktype = SOCK_STREAM; /* TCP */
		hints.ai_flags = 0;
		hints.ai_protocol = IPPROTO_TCP; /* Any protocol */

		addrinfo *addrinfoResult;
		int s = getaddrinfo(host, portCharPtr, &hints, &addrinfoResult);
		if (s != 0) {
			std::stringstream ess;
			ess << "sendPackRpcRequest(): getaddrinfo() failed: " << gai_strerror(s);

			NsMonitoring::monitorSendRpcRequestError(serviceName, serviceName, 0x807060504030201,
													 "sendPackRpcRequest(): getaddrinfo() failed"); // TODO Generate IDs // Remote method is not known here, so sending the service name twice

			throw NsException(NSE_POSITION, ess);
		}

		/* getaddrinfo() returns a list of address structures.
		   Try each address until we successfully connect(2).
		   If socket(2) (or connect(2)) fails, we (close the socket
		   and) try the next address. */

		addrinfo *rp;
		int sockfd;
		for (rp = addrinfoResult; rp != NULL; rp = rp->ai_next) {
			sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (sockfd == -1)
				continue;

			if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
				break; /* Success */

			close(sockfd);
		}

		//	cout_lock.lock ();
		//	std::cout << "Client:sendRpcRequest(): fd=" << sockfd << std::endl;
		//	cout_lock.unlock ();

		if (rp == NULL) { /* No address succeeded */
			NsMonitoring::monitorSendRpcRequestError(serviceName, serviceName, 0x807060504030201,
													 "sendPackRpcRequest(): Could not connect"); // TODO Generate IDs // Remote method is not known here, so sending the service name twice

			throw NsException(NSE_POSITION, "sendPackRpcRequest(): Could not connect");
		}
		freeaddrinfo(addrinfoResult); /* No longer needed */

		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////

		std::shared_ptr<NsSerialized> requestSerialized = _requestSerializer.serialize(request);

		uint32_t requestLen = requestSerialized->size;
		uint32_t requestLenNet = htonl(requestLen);

		//	cout_lock.lock ();
		//	std::cout << "Client:sendRpcRequest(): outgoing message len: " << requestLen << ", HEX " << std::hex << requestLen << std::dec << std::endl;
		//	std::cout << "Client:sendRpcRequest(): outgoing message len (network byte order): " << requestLenNet << std::endl;
		//	cout_lock.unlock ();

		writeBin(sockfd, &requestLenNet, sizeof(requestLenNet));

		//	cout_lock.lock ();
		//	std::cout << "Client:sendRpcRequest(): outgoing message len written" << std::endl;
		//	cout_lock.unlock ();

		writeBin(sockfd, requestSerialized->ptr, requestLen);

		NsMonitoring::monitorSendRpcRequest(serviceName, std::make_shared<std::string>(request->method()),
											0x807060504030201); // TODO Generate IDs

		//	cout_lock.lock ();
		//	std::cout << "Client:sendRpcRequest(): outgoing message written" << std::endl;
		//	cout_lock.unlock ();

		if (!request->waitForResponse()) {
			int closeResult = ::close(sockfd);
			if (closeResult != 0) {
				int e = errno;

				//		std::stringstream ess;
				std::cerr << "sendRpcRequest(): (waitForAnswer=false): Could not close socket " << sockfd << ": "
						  << ::strerror(e) << std::endl;
				//		throw NsException (NSE_POSITION, ess);
			}

			return nullptr;
		}

		//	cout_lock.lock ();
		//	std::cout << "Client:sendRpcRequest(): reading response msglen" << std::endl;
		//	cout_lock.unlock ();

		uint32_t msglen;

		{
			ssize_t msglenrcount;
			msglenrcount = read(sockfd, &msglen, sizeof(msglen));

			if (msglenrcount < ((ssize_t) sizeof(msglen))) {
				std::stringstream ess;
				ess << "sendPackRpcRequest(): Cannot read incoming message length (" << sizeof(msglen) << " bytes)"
					<< std::endl;

				NsMonitoring::monitorReceiveRpcResponseError(0x8017060504030201,
															 "sendPackRpcRequest(): Cannot read incoming message length"); // TODO Generate IDs

				throw NsException(NSE_POSITION, ess);
			}

			msglen = ntohl(msglen);

			//		cout_lock.lock ();
			//		std::cout << "Client:sendRpcRequest(): response msglen: " << msglen << ". Reading response..." << std::endl;
			//		cout_lock.unlock ();
		}

		std::shared_ptr<NsBinBuffer> buf = readBin(sockfd, msglen);

		int closeResult = ::close(sockfd);
		if (closeResult != 0) {
			int e = errno;

			//		std::stringstream ess;
			std::cerr << "sendRpcRequest(): (waitForAnswer=true): Could not close socket " << sockfd << ": "
					  << ::strerror(e) << std::endl;
			//		throw NsException (NSE_POSITION, ess);
		}

		//	cout_lock.lock ();
		//	std::cout << "Client:sendRpcRequest(): response read. Creating a shared pointer" << std::endl;
		//	cout_lock.unlock ();

		std::shared_ptr<NsSerialized> responseSerialized = buf->toPackable();

		// DEBUG
		//	std::cout << "sendRpcRequest(): responseSerialized: " << std::endl << hexdump (responseSerialized->ptr, responseSerialized->size) << std::endl;

		std::shared_ptr<NsRpcResponse> response = _responseSerializer.deserialize(responseSerialized);
		return response;
	}

	template<typename Args, typename Result>
	static std::shared_ptr<Result>
	sendRpcRequest(std::shared_ptr<std::string> serviceName, std::shared_ptr<std::string> methodName,
				   std::shared_ptr<Args> args, bool waitForResponse) throw(NsException) {
		std::shared_ptr<NsSkelRpcService> service;
		try {
			service = NsSkelRpcRegistry::instance()->getService(serviceName);
		} catch (NsException &ex) {
			std::stringstream ess;
			ess << "NsException: " << ex.what();
			throw NsException(NSE_POSITION, ess);
		}

		const char *host = service->host()->c_str();
		uint16_t port = service->port();

		//	cout_lock.lock ();
		//	std::cout << "sendRpcRequest(): Trying to connect to host " << host << ":" << port << std::endl;
		//	cout_lock.unlock ();

		NsSerializer<Args> argsSerializer;
		std::shared_ptr<NsSerialized> argsSerialized = argsSerializer.serialize(args);
		std::shared_ptr<NsRpcRequest> request = std::make_shared<NsRpcRequest>(*methodName, *argsSerialized,
																			   waitForResponse);

		std::shared_ptr<NsRpcResponse> response = sendPackRpcRequest(serviceName, host, port, request);

		//	cout_lock.lock ();
		//	std::cout << "Client: freeing responseSerialized" << std::endl;
		//	cout_lock.unlock ();
		//
		//	::free (const_cast<char *> (responseSerialized->ptr));

		std::shared_ptr<Result> result = nullptr;
		if (waitForResponse) {
			if (!response->success()) {
				std::shared_ptr<NsRpcResponseError> err = _errorSerializer.deserialize(response->result());
				//		::free (const_cast<char*> (errSerialized->ptr));

				std::shared_ptr<NsException> ex;

				std::stringstream ess;
				ess << "Exception on remote RPC side (service " << *serviceName << ", method " << *methodName << "):"
					<< std::endl << err->errorCode() << ": ";

				std::map<std::string, std::string>::iterator causeIter = err->optionalErrorParams().find("cause");
				std::map<std::string, std::string>::iterator causeNotFoundIter = err->optionalErrorParams().end();
				if (causeIter != causeNotFoundIter) {
					ess << causeIter->second;

					ex = std::make_shared<NsException>(NSE_POSITION, ess, "", causeIter->second,
													   err->errorDescription());
				} else {
					ess << err->errorDescription();

					ex = std::make_shared<NsException>(NSE_POSITION, ess);
				}

				NsMonitoring::monitorReceiveRpcResponseError(0x807060504030201,
															 "Exception on remote RPC side"); // TODO Generate IDs

				throw *ex;
			}

			// DEBUG
			//	std::cout << "sendRpcRequest(): resultSerialized: " << std::endl << hexdump (response->result ().ptr, response->result ().size) << std::endl;

			NsSerializer<Result> resultSerializer;
			result = resultSerializer.deserialize(response->result());
		}
		NsMonitoring::monitorReceiveRpcResponse(0x807060504030201); // TODO Generate IDs

		return result;
	}
}

#endif /* NSSKELRPCSERVER_H */

