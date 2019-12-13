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
 * File:   NsSkelRpcHttpServer.cpp
 * Author: dmerkushov
 *
 * Created on June 24, 2019, 5:07 PM
 */

#include "NsSkelRpcHttpServer.h"

#include <mutex>

#include <unistd.h>

#include "NsSkelRpcServer.h"
#include "NsSkelUtils.h"
#include "NsSerializer.h"
#include "NsRpcExecutor.h"
#include "sockets/socketstream.h"

using namespace std;
using namespace nanoservices;
using namespace galik::net;

extern mutex cout_lock;

NsSkelRpcHttpServer::NsSkelRpcHttpServer () : NsSkelRpcServer () {
	setPort (NsSkelRpcRegistry::instance ()->getLocalService ()->httpPort ());
}

NsSkelRpcHttpServer::~NsSkelRpcHttpServer () {
}

void NsSkelRpcHttpServer::processIncomingConnection (int dataSocketFd) try {

	socketstream sockstream (dataSocketFd);

	string method;
	sockstream >> method;

	if (method != "GET") {
		cout_lock.lock ();
		cerr << "NsSkelRpcHttpServer: Unsupported HTTP method: " << method << endl;
		cerr << "Currently, only GET is supported" << endl;
		cout_lock.unlock ();

		sockstream.close ();

		return;
	}

	string receivedRequest;
	sockstream >> receivedRequest;

	if (receivedRequest[0] != '/') {
		cout_lock.lock ();
		cerr << "NsSkelRpcHttpServer: Unsupported request string: \"" << receivedRequest << '"' << endl;
		cerr << "Request string must start with a slash (/)" << endl;
		cout_lock.unlock ();

		sockstream.close ();

		return;
	}

	string httpVersion;
	sockstream >> httpVersion;

	if (httpVersion.substr (0, 6) != "HTTP/1") {
		cout_lock.lock ();
		cerr << "NsSkelRpcHttpServer: Unsupported HTTP version: " << httpVersion << endl;
		cerr << "Currently, only HTTP/1.0 and HTTP/1.1 are supported" << endl;
		cout_lock.unlock ();

		sockstream.close ();

		return;
	}

	shared_ptr<string> requestBase64 = make_shared<string> (receivedRequest.substr (1));

	shared_ptr<NsSerialized> rpcRequestSerialized = NsSkelUtils::fromBase64 (requestBase64);
	bool waitForResponse = true;
	shared_ptr<NsSerialized> rpcResultSerialized;

	rpcResultSerialized = processRpcRequest (rpcRequestSerialized, waitForResponse);

	// DEBUG
	cout_lock.lock ();
	cout << "processIncomingConnection(): serialized result: " << endl << hexdump (rpcResultSerialized->ptr, rpcResultSerialized->size) << endl;
	cout << "processIncomingConnection(): Wait for response: " << waitForResponse << endl;
	cout_lock.unlock ();

	if (waitForResponse) {
		shared_ptr<string> resultBase64 = NsSkelUtils::toBase64 (rpcResultSerialized);

		// DEBUG
		cout_lock.lock ();
		cout << "processIncomingConnection(): base64 result: " << *resultBase64 << endl;
		cout_lock.unlock ();

		sockstream << "HTTP/1.1 200 OK\n";
		sockstream << "Content-Type: application/base64\n";
		sockstream << "Access-Control-Allow-Origin: *\n";
		sockstream << "Content-Length: " << resultBase64->size () << "\n\n";
		sockstream << *resultBase64;

		// To send raw binary data in response:
		//		sockstream << "Content-Type: application/octet-stream\n";
		//		sockstream << "Content-Length: " << rpcResultSerialized->size << "\n\n";
		//		for (uint32_t i = 0; i < rpcResultSerialized->size; i++) {
		//			sockstream << rpcResultSerialized->ptr[i];
		//		}

		sockstream.flush ();
	}

	// DEBUG
	cout_lock.lock ();
	cout << "processIncomingConnection(): closing socket" << endl;
	cout_lock.unlock ();

	sockstream.close ();

} catch (NsException &ex) {

	cout_lock.lock ();
	cerr << "processIncomingConnection(): NsException: " << ex.what () << endl;
	cout_lock.unlock ();
	::close (dataSocketFd);

} catch (std::exception &ex) {

	cout_lock.lock ();
	cerr << "processIncomingConnection(): std::exception: " << ex.what () << endl;
	cout_lock.unlock ();
	::close (dataSocketFd);

} catch (...) {

	cout_lock.lock ();
	cerr << "processIncomingConnection(): Unexpected failure" << endl;
	cout_lock.unlock ();
	::close (dataSocketFd);

}
