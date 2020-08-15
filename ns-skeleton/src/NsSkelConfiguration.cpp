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
 * File:   NsSkelConfiguration.cpp
 * Author: dmerkushov
 *
 * Created on February 28, 2019, 3:12 PM
 */

#include "NsSkelConfiguration.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <iostream>

#include <cstring>

#include "NsException.h"
#include "AbstractConfig.h"

#include "NsSkelRpcRegistry.h"

using namespace std;
using namespace nanoservices;

const std::string CONFIGPATH_SERVICES_BASE = "/etc/nanoservices/local/";
const std::string CONFIGPATH_GLOBAL = "/etc/nanoservices/global";
const std::string CONFIGPATH_EXTEN = ".conf.json";

static shared_ptr<NsSkelConfiguration> _instanceOfNsSkelConfiguration_doNotUseDirectly;

void nsSkelConfig(const string &serviceName, const string &configName) {
	NsSkelConfiguration *config = new NsSkelConfiguration(serviceName, configName);
	_instanceOfNsSkelConfiguration_doNotUseDirectly = shared_ptr<NsSkelConfiguration>(config);
}


NsSkelConfiguration::NsSkelConfiguration(const string &serviceName, const string &configName) {
	_serviceName = make_shared<string>(serviceName);
	_configName = make_shared<string>(configName);

	// URI come from CMake
	AbstractConfig::init(URI);
	
	// HEAD come from CMake
	auto confData = AbstractConfig::instance()->read(HEAD);
	
	if(confData->type() != JSON_OBJECT) {
		stringstream ess;
		ess << "Empty configuration";
		throw NsException(NSE_POSITION, ess);		
	}
	
	_services = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(confData);
	
	if(_services->find(configName) == _services->end()) {
		stringstream ess;
		ess << "No configuration for service " << configName;
		throw NsException(NSE_POSITION, ess);
	}
	
	_configuration = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(_services->at(configName));
}

shared_ptr<NsSkelConfiguration> NsSkelConfiguration::instance() {
	if (!_instanceOfNsSkelConfiguration_doNotUseDirectly) {
		throw NsException(NSE_POSITION, "Call of NsSkelConfiguration::getInstance() before calling nsSkelConfig()");
	}
	return _instanceOfNsSkelConfiguration_doNotUseDirectly;
}

shared_ptr<string> NsSkelConfiguration::getServiceName() {
	shared_ptr<string> returned = _serviceName;
	return returned;
}

shared_ptr<string> NsSkelConfiguration::getConfigName() {
	shared_ptr<string> returned = _configName;
	return returned;
}

bool NsSkelConfiguration::hasParameter(string paramName) {
	return (_configuration->find(paramName) != _configuration->end());
}

NsSkelJsonObjectPtr NsSkelConfiguration::getServices() {
	return _services;
}

namespace nanoservices {

	template<>
	NsSkelJsonPtr NsSkelConfiguration::getParameter<NsSkelJsonPtr>(std::string paramName,
		NsSkelJsonPtr defaultVal) {
		if (!hasParameter(paramName)) {
			return defaultVal;
		}
		return _configuration->find(paramName)->second;
	}
}
