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
 * File:   NsMonitoring.cpp
 * Author: dmerkushov
 *
 * Created on July 8, 2019, 2:56 PM
 */

#include "NsMonitoring.h"

#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/time.h>
#include <byteswap.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "NsSerializer.h"
#include "NsSkeleton.h"
#include "NsSkelConfigException.h"

using namespace std;
using namespace nanoservices;

NsMonIp4Address::NsMonIp4Address (uint32_t ipv4) {
	_ipv4 = ipv4;
}

NsMonIp4Address::NsMonIp4Address (const std::string& ipv4str) {
	int n1, n2, n3, n4;
	::sscanf (ipv4str.c_str (), "%d.%d.%d.%d", &n1, &n2, &n3, &n4);
	_ipv4 = (n1 << 24) + (n2 << 16) + (n3 << 8) + n4;
}

NsMonIp4Address::NsMonIp4Address (const char * ipv4char) {
	int n1, n2, n3, n4;
	::sscanf (ipv4char, "%d.%d.%d.%d", &n1, &n2, &n3, &n4);
	_ipv4 = (n1 << 24) + (n2 << 16) + (n3 << 8) + n4;
}

uint32_t NsMonIp4Address::ipv4 () const {
	return _ipv4;
}

bool NsMonitoring::enabled () {
	return NsSkelConfiguration::instance ()->getParameter<bool> (CONFIG_PARAMNAME_NSMONITORING_ENABLED, false);
}

stringstream * NsMonitoring::sendMonStart () {

	::timespec ts;
	::clock_gettime (CLOCK_REALTIME, &ts);

	uint64_t millis = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

	//DEBUG
	//	cout << "NsMonitoring::sendMonStart(): millis from epoch: " << millis << endl;

	NsMonitoringType<uint64_t> millisMon (millis);

	shared_ptr<string> serviceName = NsSkeleton::serviceName ();
	NsMonitoringType<string> serviceNameMon (serviceName);

	stringstream * os = new stringstream ();

	os->write ("\xFF\xEE\xEE\xDD", 4);

	millisMon.writeTo (os);
	serviceNameMon.writeTo (os);

	//DEBUG
	//	cout << "NsMonitoring::sendMonStart(): serviceNameMon: ";
	//	serviceNameMon.writeTo (&cout);
	//	cout << endl;

	return os;
}

void NsMonitoring::doSendMon (std::stringstream *os) {
	int en;

	int sock = ::socket (AF_INET, SOCK_DGRAM, 0);
	en = errno;

	if (sock < 0) {
		cerr << "NsMonitoring::doSendMon(): Cannot open sending UDP socket: " << ::strerror (en) << endl;
		return;
	}

	string hostnameStr = NsSkelConfiguration::instance ()->getParameter<string> (CONFIG_PARAMNAME_NSMONITORING_HOST);
	uint16_t port = (uint16_t) NsSkelConfiguration::instance ()->getParameter<int64_t> (CONFIG_PARAMNAME_NSMONITORING_PORT_UDP);

	//DEBUG
	//	cout << "Sending monitoring data to: " << hostnameStr << ":" << port << endl;

	::sockaddr_in servaddr;
	::memset (&servaddr, '\0', sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = ::inet_addr (hostnameStr.c_str ());
	servaddr.sin_port = ::htons (port);

	const string msg (os->str ());
	delete os;

	//DEBUG
	//	cout << "NsMonitoring::doSendMon(): Sending:" << endl;
	//	cout << hexdump (msg.c_str (), msg.length ());

	int sendresult = ::sendto (sock, msg.c_str (), msg.length (), 0, (sockaddr*) & servaddr, sizeof (servaddr));
	en = errno;

	if (sendresult < 0) {
		cerr << "NsMonitoring::doSendMon(): Cannot send monitoring data: " << ::strerror (en) << endl;
	}

	close (sock);
}

void NsMonitoring::monitorStartup () {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	uint8_t command = EVENT_STARTUP_NOHTTP;
	if (NsSkelRpcRegistry::instance ()->getLocalService ()->httpEnabled ()) {
		command = EVENT_STARTUP_HTTP;
	}

	shared_ptr<string> address = NsSkelRpcRegistry::instance ()->getLocalService ()->host ();
	uint16_t tcpPort = NsSkelRpcRegistry::instance ()->getLocalService ()->port ();
	uint16_t httpPort;
	if (command == EVENT_STARTUP_HTTP) {
		httpPort = NsSkelRpcRegistry::instance ()->getLocalService ()->httpPort ();
	}

	NsMonitoringArray methodsNma;
	shared_ptr<vector<string> > methods = NsSkelRpcRegistry::instance ()->methods ();
	for (vector<string>::iterator methodsIt = methods->begin (); methodsIt != methods->end (); methodsIt++) {
		methodsNma.add<string> (*methodsIt);
	}

	if (command == EVENT_STARTUP_NOHTTP) {
		send (command, *address, tcpPort, methodsNma);
	} else {
		send (command, *address, tcpPort, httpPort, methodsNma);
	}
}

void NsMonitoring::monitorShutdown () {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	send (EVENT_SHUTDOWN);
}

void NsMonitoring::monitorSendRpcRequest (shared_ptr<string> remoteServiceName, std::shared_ptr<std::string> remoteMethod, const uint64_t requestId) {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	send (EVENT_SENDRPCREQUEST, *remoteServiceName, *remoteMethod, requestId);
}

void NsMonitoring::monitorSendRpcRequestError (shared_ptr<string> remoteServiceName, std::shared_ptr<std::string> remoteMethod, const uint64_t requestId, const char *errorDescription) {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	send (EVENT_SENDRPCREQUEST_ERROR, *remoteServiceName, *remoteMethod, requestId, string (errorDescription));
}

void NsMonitoring::monitorReceiveRpcResponse (const uint64_t requestId) {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	send (EVENT_RECEIVERPCRESPONSE, requestId);
}

void NsMonitoring::monitorReceiveRpcResponseError (const uint64_t requestId, const char *errorDescription) {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	send (EVENT_RECEIVERPCRESPONSE_ERROR, requestId, string (errorDescription));
}

void NsMonitoring::monitorReceiveRpcRequest (const uint64_t requestId, shared_ptr<string> method) {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	send (EVENT_RECEIVERPCREQUEST, requestId, *method);
}

void NsMonitoring::monitorReceiveRpcRequestError (const char *errorDescription) {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	send (EVENT_RECEIVERPCREQUEST_ERROR, string (errorDescription));
}

void NsMonitoring::monitorSendRpcResponse (const uint64_t requestId) {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	send (EVENT_SENDRPCRESPONSE, requestId);
}

void NsMonitoring::monitorSendRpcResponseError (const char *errorDescription) {
	if (!NsMonitoring::enabled ()) {
		return;
	}

	send (EVENT_SENDRPCRESPONSE_ERROR, string (errorDescription));
}
