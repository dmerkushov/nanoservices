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
 * File:   NsSkelRpcService.cpp
 * Author: dmerkushov
 *
 * Created on March 19, 2019, 6:25 PM
 */

#include <string>
#include <sstream>

#include "NsSkelRpcService.h"
#include "NsException.h"
#include "NsSkelJson.h"

using namespace std;
using namespace nanoservices;

NsSkelRpcService::NsSkelRpcService(shared_ptr<string> serviceName, shared_ptr<string> host, uint16_t port,
								   bool httpEnabled = false, uint16_t httpPort = 0) :
		_serviceName(serviceName),
		_host(host),
		_port(port),
		_httpEnabled(httpEnabled),
		_httpPort(httpPort) {
}

NsSkelRpcService::NsSkelRpcService(shared_ptr<string> serviceName, NsSkelJsonPtr serviceJson) throw(NsException) {
	NsSkelJsonObject serviceJsonObj = fromNsSkelJsonPtr<NsSkelJsonObject>(serviceJson);

	if (serviceJsonObj.find("host") == serviceJsonObj.end()) {
		throw (NsException(NSE_POSITION, "No host in service description JSON"));
	}
	if (serviceJsonObj.find("port") == serviceJsonObj.end()) {
		throw (NsException(NSE_POSITION, "No port in service description JSON"));
	}

	_serviceName = serviceName;
	_host = make_shared<string>(fromNsSkelJsonPtr<std::string>(serviceJsonObj["host"]));
	_port = fromNsSkelJsonPtr<uint16_t>(serviceJsonObj["port"]);

	if (serviceJsonObj.find("http-enabled") != serviceJsonObj.end() &&
		fromNsSkelJsonPtr<bool>(serviceJsonObj["http-enabled"])) {
		if (serviceJsonObj.find("http-port") == serviceJsonObj.end()) {
			throw (NsException(NSE_POSITION, "HTTP is enabled in service description JSON, but no HTTP port set"));
		}
		_httpEnabled = true;
		_httpPort = fromNsSkelJsonPtr<uint16_t>(serviceJsonObj["http-port"]);
	} else {
		_httpEnabled = false;
		_httpPort = 0;
	}
}

NsSkelRpcService::~NsSkelRpcService() {
}

shared_ptr<string> NsSkelRpcService::serviceName() {
	return _serviceName;
}

shared_ptr<string> NsSkelRpcService::host() {
	return _host;
}

uint16_t NsSkelRpcService::port() {
	return _port;
}

uint16_t NsSkelRpcService::httpPort() {
	return _httpPort;
}

bool NsSkelRpcService::httpEnabled() {
	return _httpEnabled;
}
