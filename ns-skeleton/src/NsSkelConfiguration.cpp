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

#include "NsSkelJson.h"
#include "NsException.h"
#include "NsSkelRpcRegistry.h"

using namespace std;
using namespace nanoservices;

const std::string CONFIGPATH_SERVICES_BASE = "/etc/nanoservices/local/";
const std::string CONFIGPATH_GLOBAL = "/etc/nanoservices/global";
const std::string CONFIGPATH_EXTEN = ".conf.json";

static shared_ptr<NsSkelConfiguration> _instanceOfNsSkelConfiguration_doNotUseDirectly;

void nsSkelConfig(const string &serviceName, const string &configName) throw(NsException) {
	NsSkelConfiguration *config = new NsSkelConfiguration(serviceName, configName);
	_instanceOfNsSkelConfiguration_doNotUseDirectly = shared_ptr<NsSkelConfiguration>(config);
}

NsSkelConfiguration::NsSkelConfiguration(const string &serviceName, const string &configName) throw(NsException) {
	_serviceName = make_shared<string>(serviceName);
	_configName = make_shared<string>(configName);

	// Local service conf
	stringstream configFilenameS;
	configFilenameS << CONFIGPATH_SERVICES_BASE;
	configFilenameS << configName;
	configFilenameS << CONFIGPATH_EXTEN;
	string configFilename = configFilenameS.str();
	ifstream is(configFilename);
	if (is.fail()) {
		stringstream ess;
		ess << "Couldn't open configuration for service " << serviceName << " (file " << configFilename << "): "
			<< strerror(errno);
		throw NsException(NSE_POSITION, ess);
	}
	NsSkelJsonParser parser;
	_configuration = parser.typedParse<NsSkelJsonObjectPtr>(is);

	// DEBUG
	//	cout << "NsSkelConfiguration: local configuration (file " << configFilename << ") before adding: " << _configuration->serialize () << endl;

	// Global conf
	stringstream globalConfigFilenameS;
	globalConfigFilenameS << CONFIGPATH_GLOBAL << CONFIGPATH_EXTEN;
	string globalConfigFilename = globalConfigFilenameS.str();
	ifstream gis(globalConfigFilename);
	if (gis.fail()) {
		stringstream ess;
		ess << "Couldn't open global configuration (file " << globalConfigFilename << "): " << strerror(errno);
		throw NsException(NSE_POSITION, ess);
	}
	//	NsSkelJsonPtr gconfP = parser.parse (gis);
	NsSkelJsonObjectPtr gconf = parser.typedParse<NsSkelJsonObjectPtr>(gis);

	// DEBUG
	//	cout << "NsSkelConfiguration: global configuration (file " << globalConfigFilename << ") before adding: " << gconf->serialize () << endl;

	// Add the global conf to the local one, excluding repeated members
	_configuration->add(*gconf);

	// DEBUG
	//	cout << "NsSkelConfiguration: local configuration (file " << globalConfigFilename << ") after adding: " << _configuration->serialize () << endl;
}

shared_ptr<NsSkelConfiguration> NsSkelConfiguration::instance() throw(NsException) {
	if (!_instanceOfNsSkelConfiguration_doNotUseDirectly) {
		throw NsException(NSE_POSITION, "Call of NsSkelConfiguration::getInstance() before calling nsSkelConfig()");
	}
	return _instanceOfNsSkelConfiguration_doNotUseDirectly;
}

shared_ptr<string> NsSkelConfiguration::getServiceName() throw(NsException) {
	shared_ptr<string> returned = _serviceName;
	return returned;
}

shared_ptr<string> NsSkelConfiguration::getConfigName() throw(NsException) {
	shared_ptr<string> returned = _configName;
	return returned;
}

bool NsSkelConfiguration::hasParameter(string paramName) {
	return (_configuration->find(paramName) != _configuration->end());
}

namespace nanoservices {

template<>
NsSkelJsonPtr NsSkelConfiguration::getParameter<NsSkelJsonPtr>(const std::string &paramName,
															   NsSkelJsonPtr defaultVal) throw(nanoservices::NsException) {
	if (!hasParameter(paramName)) {
		return defaultVal;
	}
	return _configuration->find(paramName)->second;
}
}
