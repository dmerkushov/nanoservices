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
 * File:   NsSkelRpcReplierRegistry.cpp
 * Author: dmerkushov
 *
 * Created on March 12, 2019, 2:23 PM
 */

#include "NsSkelRpcRegistry.h"

#include <exception>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "NsException.h"
#include "NsSkelRpcService.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelConfiguration.h"
#include "NsSkelRpcServer.h"

using namespace std;
using namespace nanoservices;

static mutex _repliersMapMutex;
static mutex _servicesMapMutex;
static mutex _serversVectorMutex;

static shared_ptr<NsSkelRpcRegistry> _NsSkelRpcRegistry_instance_doNotUseDirectly;

shared_ptr<NsSkelRpcRegistry> NsSkelRpcRegistry::instance() {
	if (!_NsSkelRpcRegistry_instance_doNotUseDirectly) {
		_NsSkelRpcRegistry_instance_doNotUseDirectly = shared_ptr<NsSkelRpcRegistry>(new NsSkelRpcRegistry());
	}
	return _NsSkelRpcRegistry_instance_doNotUseDirectly;
}

NsSkelRpcRegistry::NsSkelRpcRegistry() {
}

NsSkelRpcRegistry::~NsSkelRpcRegistry() {
}

void NsSkelRpcRegistry::registerReplier(shared_ptr<NsSkelRpcReplierInterface> replier) {
	if (!replier) {
		throw NsException(NSE_POSITION, "Empty shared_ptr to the replier to register");
	}

	shared_ptr<string> method = replier->methodName();

	_repliersMapMutex.lock();
	_repliers[*method] = replier;
	_repliersMapMutex.unlock();
}

void NsSkelRpcRegistry::unregisterReplier(shared_ptr<string> methodName) {
	if (!methodName) {
		throw NsException(NSE_POSITION, "Empty shared_ptr to the replier method name to unregister");
	}

	_repliersMapMutex.lock();
	std::map<std::string, std::shared_ptr<NsSkelRpcReplierInterface> >::const_iterator it;
	try {
		it = _repliers.find(*methodName);
	} catch (out_of_range &ex) {
		// Replier not found among the registered ones
		return;
	} catch (exception &ex) {
		stringstream errs;
		errs << "Unexpected exception when looking for replier for method " << *methodName << ": " << ex.what();

		_repliersMapMutex.unlock();
		throw NsException(NSE_POSITION, errs.str());
	}
	if (it == _repliers.end()) {
		return;
	}
	_repliers.erase(it);
	_repliersMapMutex.unlock();
}

shared_ptr<NsSkelRpcReplierInterface> NsSkelRpcRegistry::getReplier(shared_ptr<string> methodName) {
	shared_ptr<NsSkelRpcReplierInterface> replier;

	_repliersMapMutex.lock();
	try {
		replier = _repliers.at(*methodName);
	} catch (out_of_range &ex) {
		stringstream errs;
		errs << "No replier found for method " << *methodName << ": " << ex.what();

		_repliersMapMutex.unlock();
		throw NsException(NSE_POSITION, errs.str());
	} catch (exception &ex) {
		stringstream errs;
		errs << "Unexpected exception when looking for replier for method " << methodName << ": " << ex.what();

		_repliersMapMutex.unlock();
		throw NsException(NSE_POSITION, errs.str());
	}
	_repliersMapMutex.unlock();

	return replier;
}

shared_ptr<vector<string> > NsSkelRpcRegistry::methods() {
	shared_ptr<vector<string> > methods = make_shared<vector<string> >();

	_repliersMapMutex.lock();
	for (auto repliersIt = _repliers.begin(); repliersIt != _repliers.end(); repliersIt++) {
		methods->push_back(string(repliersIt->first));
	}
	_repliersMapMutex.unlock();

	return methods;
}

void NsSkelRpcRegistry::prepareServicesMap() {
	NsSkelJsonPtr servicesJson;

	_servicesMapMutex.lock();

	if (_servicesMapReady) {
		_servicesMapMutex.unlock();
		return;
	}

	try {
		servicesJson = NsSkelConfiguration::instance()->getServices();
	} catch (NsException &ex) {
		stringstream ess;
		ess << "getService(): NsException: " << ex.what();
		throw (NsException(NSE_POSITION, ess));
	}

	if (!servicesJson) {
		throw (NsException(NSE_POSITION, "getService(): known-services not found"));
	} else if (servicesJson->type() != NsSkelJsonValueType::JSON_OBJECT) {
		stringstream ess;
		ess << "getService(): known-services is not an object: " << verboseNsSkelJsonType(servicesJson->type());
		throw (NsException(NSE_POSITION, ess));
	}

	NsSkelJsonObjectPtr servicesJsonMap = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(servicesJson);

	for (auto it = servicesJsonMap->begin(); it != servicesJsonMap->end(); ++it) {
		shared_ptr<NsSkelRpcService> service = make_shared<NsSkelRpcService>(make_shared<string>(it->first), it->second);
		_services[*(service->serviceName())] = service;
	}

	_services[*(NsSkelConfiguration::instance()->getServiceName())] = _services[*(NsSkelConfiguration::instance()->getConfigName())];

	_servicesMapReady = true;
	_servicesMapMutex.unlock();
}

shared_ptr<NsSkelRpcService> NsSkelRpcRegistry::getService(shared_ptr<string> serviceName) {
	if (!serviceName) {
		throw (NsException(NSE_POSITION, "Service name pointer is empty"));
	}

	prepareServicesMap();

	shared_ptr<NsSkelRpcService> service;
	string serviceNameStr(*serviceName);

	_servicesMapMutex.lock();
	if (_services.count(serviceNameStr) <= 0) {
		stringstream errs;
		errs << "No service description found for service name \"" << *serviceName << "\"";

		_servicesMapMutex.unlock();
		throw NsException(NSE_POSITION, errs);
	}

	map<string, shared_ptr<NsSkelRpcService> >::iterator findSName;

	try {
		findSName = _services.find(serviceNameStr);
	} catch (out_of_range &ex) {
		stringstream errs;
		errs << "std::out_of_range when looking for service name \"" << *serviceName << "\": " << ex.what();

		_servicesMapMutex.unlock();
		throw NsException(NSE_POSITION, errs);
	} catch (std::exception &ex) {
		stringstream errs;
		errs << "std::exception when looking for service for name \"" << serviceName << "\": " << ex.what();

		_servicesMapMutex.unlock();
		throw NsException(NSE_POSITION, errs);
	} catch (...) {
		stringstream errs;
		errs << "Caught something unexpected when looking for service for name \"" << serviceName << "\"";

		_servicesMapMutex.unlock();
		throw NsException(NSE_POSITION, errs);
	}

	if (!(findSName != _services.end())) { // May compare found-iterator and end() only by operator!=
		stringstream errs;
		errs << "No service description found for service name \"" << *serviceName << "\"";

		_servicesMapMutex.unlock();
		throw NsException(NSE_POSITION, errs);
	}

	service = findSName->second;
	_servicesMapMutex.unlock();

	if (!service) {
		stringstream errs;
		errs << "The found service description pointer is empty for service name \"" << *serviceName
			 << "\" (probably the service is not described in known-services section of the configuration)";

		_servicesMapMutex.unlock();
		throw NsException(NSE_POSITION, errs);
	}

	return service;
}

void NsSkelRpcRegistry::initialize() {
}

shared_ptr<NsSkelRpcService> NsSkelRpcRegistry::getLocalService() {
	shared_ptr<string> serviceName = NsSkelConfiguration::instance()->getServiceName();

	return NsSkelRpcRegistry::instance()->getService(serviceName);
}

void NsSkelRpcRegistry::registerServer(std::shared_ptr<NsSkelRpcServer> server) {
	if (!server) {
		return;
	}

	_serversVectorMutex.lock();
	_servers.push_back(server);
	_serversVectorMutex.unlock();
}

void NsSkelRpcRegistry::unregisterServer(shared_ptr<NsSkelRpcServer> server) {
	if (!server) {
		return;
	}

	_serversVectorMutex.lock();
#if (!defined(__GNUC__)) || (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ > 7)
	vector<shared_ptr<NsSkelRpcServer> >::const_iterator it = _servers.begin();
#else
# warning Building with an old GCC version (4.7 or older)! Must use the old C++ STL, not completely compatible with C++11
	vector<shared_ptr<NsSkelRpcServer> >::iterator it = _servers.begin();
#endif
	while (it != _servers.end() && (*it) != server) {
		it++;
	}
	if ((*it) == server) {
		_servers.erase(it);
	}
	_serversVectorMutex.unlock();
}

void NsSkelRpcRegistry::startupServers() {
	_serversVectorMutex.lock();
	vector<shared_ptr<NsSkelRpcServer> >::iterator it;
	try {
		for (it = _servers.begin(); it != _servers.end(); it++) {
			it->get()->startup();
		}
	} catch (NsException &e) {
		_serversVectorMutex.unlock();
		stringstream ess;
		ess << "NsException in NsSkelRpcRegistry::startupServers()";
		if ((*it) != nullptr) {
			ess << " (when starting server on port " << it->get()->port() << ")";
		}
		ess << ": " << e.what();
		throw (NsException(NSE_POSITION, ess));
	} catch (std::exception &e) {
		_serversVectorMutex.unlock();
		stringstream ess;
		ess << "std::exception in NsSkelRpcRegistry::startupServers()";
		if ((*it) != nullptr) {
			ess << " (when starting server on port " << it->get()->port() << ")";
		}
		ess << ": " << e.what();
		throw (NsException(NSE_POSITION, ess));
	} catch (...) {
		_serversVectorMutex.unlock();
		throw (NsException(NSE_POSITION, "Unexpected in NsSkelRpcRegistry::startupServers()"));
	}
	_serversVectorMutex.unlock();
}

void NsSkelRpcRegistry::shutdownServers() {
	_serversVectorMutex.lock();
	vector<shared_ptr<NsSkelRpcServer> >::reverse_iterator it;
	try {
		for (it = _servers.rbegin(); it != _servers.rend(); it++) {
			it->get()->shutdown();
		}
	} catch (NsException &e) {
		_serversVectorMutex.unlock();
		stringstream ess;
		ess << "NsException in NsSkelRpcRegistry::shutdown()";
		if ((*it) != nullptr) {
			ess << " (when shutting down server on port " << it->get()->port() << ")";
		}
		ess << ": " << e.what();
		throw (NsException(NSE_POSITION, ess));
	} catch (std::exception &e) {
		_serversVectorMutex.unlock();
		stringstream ess;
		ess << "std::exception in NsSkelRpcRegistry::shutdown()";
		if ((*it) != nullptr) {
			ess << " (when shutting down server on port " << it->get()->port() << ")";
		}
		ess << ": " << e.what();
		throw (NsException(NSE_POSITION, ess));
	} catch (...) {
		_serversVectorMutex.unlock();
		throw (NsException(NSE_POSITION, "Unexpected in NsSkelRpcRegistry::shutdown()"));
	}
	_serversVectorMutex.unlock();
}

shared_ptr<vector<shared_ptr<NsSkelRpcServer> > > NsSkelRpcRegistry::servers() {
	shared_ptr<vector<shared_ptr<NsSkelRpcServer> > > toRet = make_shared<std::vector<std::shared_ptr<NsSkelRpcServer> > >();
	_serversVectorMutex.lock();
	vector<shared_ptr<NsSkelRpcServer> >::iterator it;
	try {
		for (it = _servers.begin(); it != _servers.end(); it++) {
			toRet->push_back(*it);
		}
	} catch (NsException &e) {
		_serversVectorMutex.unlock();
		stringstream ess;
		ess << "NsException in NsSkelRpcRegistry::shutdown()";
		if ((*it) != nullptr) {
			ess << " (when shutting down server on port " << it->get()->port() << ")";
		}
		ess << ": " << e.what();
		throw (NsException(NSE_POSITION, ess));
	} catch (std::exception &e) {
		_serversVectorMutex.unlock();
		stringstream ess;
		ess << "std::exception in NsSkelRpcRegistry::shutdown()";
		if ((*it) != nullptr) {
			ess << " (when shutting down server on port " << it->get()->port() << ")";
		}
		ess << ": " << e.what();
		throw (NsException(NSE_POSITION, ess));
	} catch (...) {
		_serversVectorMutex.unlock();
		throw (NsException(NSE_POSITION, "Unexpected in NsSkelRpcRegistry::shutdown()"));
	}
	_serversVectorMutex.unlock();

	return toRet;
}
