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

using namespace std;
using namespace nanoservices;
using namespace httplib;

extern mutex cout_lock;

NsSkelRpcHttpServer::NsSkelRpcHttpServer() : NsSkelRpcServer() {
	setPort(NsSkelRpcRegistry::instance()->getLocalService()->httpPort());
	_serverptr = make_shared<Server>();

	/*
	 * Set method on path /[same base64] 
	 *   in base64 encoded same method call, that can be received from TCP
	 */
	_serverptr->Get(R"(/((?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$))", [&](const Request& req, Response& res) {
		shared_ptr<string> requestBase64 = make_shared<string>(req.matches[1]);
		shared_ptr<NsSerialized> rpcRequestSerialized = NsSkelUtils::fromBase64(requestBase64);
		bool waitForResponse = true;
		shared_ptr<NsSerialized> rpcResultSerialized;

		rpcResultSerialized = processRpcRequest(rpcRequestSerialized, waitForResponse);
#ifndef RELEASE
		// DEBUG
		cout_lock.lock();
		cout << "processIncomingConnection(): serialized result: " << endl
			 << hexdump(rpcResultSerialized->ptr, rpcResultSerialized->size) << endl;
		cout << "processIncomingConnection(): Wait for response: " << waitForResponse << endl;
		cout_lock.unlock();
#endif
		if (waitForResponse) {
			shared_ptr<string> resultBase64 = NsSkelUtils::toBase64(rpcResultSerialized);

#ifndef RELEASE
			// DEBUG
			cout_lock.lock();
			cout << "processIncomingConnection(): base64 result: " << *resultBase64 << endl;
			cout_lock.unlock();
#endif
			res.set_content(*resultBase64, "application/base64");
			res.set_header("Access-Control-Allow-Origin", "*");
		}
	});
	_serverptr->Get("/methods", [&](const Request& req, Response& res) {
		auto methods_list = NsSkelRpcRegistry::instance()->methods();
		string content;
		for(auto method : *methods_list) {
			auto replier = NsSkelRpcRegistry::instance()->getReplier(make_shared<string>(method));
			content += replier->getReturnType()->getName() + " " + method + "(" + replier->getArgsType()->getName() +")\n";
			content += "// Received from " + *(NsSkelRpcRegistry::instance()->getLocalService()->serviceName()) + "\n";
			content += replier->getArgsType()->getDefinition() + "\n";
			content += replier->getReturnType()->getDefinition() + "\n";
		}
		res.set_content(content, "text/plain");
	});
}

NsSkelRpcHttpServer::~NsSkelRpcHttpServer() {
}

void NsSkelRpcHttpServer::startup() {
	_serverThread = std::thread([&]() {this->_serverptr->listen(this->host()->c_str(), this->port());});
	_serverThread.detach();
}

void NsSkelRpcHttpServer::shutdown() {
	if(_serverActive) {
		_serverptr->stop();
	}
	_serverActive = false;
}
