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
 * File:   NsSkeleton.cpp
 * Author: dmerkushov
 *
 * Created on June 21, 2019, 4:03 PM
 */

#include "NsSkeleton.h"

#include <memory>
#include <thread>
#include <string>
#include <sstream>

// DEBUG
#include <iostream>

#include "NsException.h"
#include "NsSkelConfiguration.h"
#include "NsSkelRpcRegistry.h"
#include "NsSkelRpcReplierInterface.h"
#include "NsSkelRpcServer.h"
#include "NsSkelRpcHttpServer.h"
#include "NsMonitoring.h"
#include "NsCmdLineParameters.h"

using namespace std;
using namespace nanoservices;

void NsSkeleton::init(const std::string &serviceName, int argc, char **argv) try {

	string configNameResult = serviceName;

	nsSkelConfig(serviceName, configNameResult);

	NsSkelJsonPtr paramKeys = nullptr;
	if (NsSkelConfiguration::instance()->hasParameter("param-keys")) {
		paramKeys = NsSkelConfiguration::instance()->getParameter<NsSkelJsonPtr>("param-keys");
	}
	auto optionsDef = getOptionDefinitions(paramKeys);

	NsCmdLineParameters::init(optionsDef, argc, argv);

	shared_ptr<NsSkelRpcServer> server = make_shared<NsSkelRpcServer>();
	NsSkelRpcRegistry::instance()->registerServer(server);

	// DEBUG
	cout << "RPC server created" << endl;

	if (NsSkelRpcRegistry::instance()->getLocalService()->httpPort() != 0) {
		// DEBUG
		cout << "HTTP is enabled in service description on port: "
			 << NsSkelRpcRegistry::instance()->getLocalService()->httpPort() << ", so will create an HTTP server"
			 << endl;

		shared_ptr<NsSkelRpcHttpServer> httpServer = make_shared<NsSkelRpcHttpServer>();
		NsSkelRpcRegistry::instance()->registerServer(httpServer);

		// DEBUG
		cout << "HTTP server created" << endl;
	}
} catch (NsException &ex) {
	stringstream ess;
	ess << "NsSkeleton::init(): NsException: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (std::exception &ex) {
	stringstream ess;
	ess << "NsSkeleton::init(): std::exception: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (...) {
	throw NsException(NSE_POSITION, "NsSkeleton::init(): Unexpected failure");
}

void NsSkeleton::startup() try {

	NsSkelRpcRegistry::instance()->startupServers();

	NsMonitoring::monitorStartup();
} catch (NsException &ex) {
	stringstream ess;
	ess << "NsSkeleton::startup(): NsException: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (std::exception &ex) {
	stringstream ess;
	ess << "NsSkeleton::startup(): std::exception: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (...) {
	throw NsException(NSE_POSITION, "NsSkeleton::startup(): Unexpected failure");
}

void NsSkeleton::shutdown() try {

	NsSkelRpcRegistry::instance()->shutdownServers();

	NsMonitoring::monitorShutdown();
} catch (NsException &ex) {
	stringstream ess;
	ess << "NsSkeleton::shutdown(): NsException: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (std::exception &ex) {
	stringstream ess;
	ess << "NsSkeleton::shutdown(): std::exception: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (...) {
	throw NsException(NSE_POSITION, "NsSkeleton::shutdown(): Unexpected failure");
}

void NsSkeleton::registerReplier(std::shared_ptr<NsSkelRpcReplierInterface> replier) try {

	NsSkelRpcRegistry::instance()->registerReplier(replier);
} catch (NsException &ex) {
	stringstream ess;
	ess << "NsSkeleton::registerReplier(): NsException: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (std::exception &ex) {
	stringstream ess;
	ess << "NsSkeleton::registerReplier(): std::exception: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (...) {
	throw NsException(NSE_POSITION, "NsSkeleton::registerReplier(): Unexpected failure");
}

void NsSkeleton::unregisterReplier(std::shared_ptr<std::string> methodName) try {

	NsSkelRpcRegistry::instance()->unregisterReplier(methodName);
} catch (NsException &ex) {
	stringstream ess;
	ess << "NsSkeleton::unregisterReplier(): NsException: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (std::exception &ex) {
	stringstream ess;
	ess << "NsSkeleton::unregisterReplier(): std::exception: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (...) {
	throw NsException(NSE_POSITION, "NsSkeleton::unregisterReplier(): Unexpected failure");
}

std::shared_ptr<std::string> NsSkeleton::serviceName() try {
	shared_ptr<string> returned = NsSkelConfiguration::instance()->getServiceName();
	if (NsCmdLineParameters::instance()->isParam("name")) {
		returned = make_shared<string>(NsCmdLineParameters::instance()->paramValue("name"));
	}
	return returned;
} catch (NsException &ex) {
	stringstream ess;
	ess << "NsSkeleton::serviceName(): NsException: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (std::exception &ex) {
	stringstream ess;
	ess << "NsSkeleton::serviceName(): std::exception: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (...) {
	throw NsException(NSE_POSITION, "NsSkeleton::serviceName(): Unexpected failure");
}

#include <chrono>

#if __cplusplus < 201402L

constexpr std::chrono::milliseconds operator ""ms(unsigned long long ms)
{
    return std::chrono::milliseconds(ms);
}

#endif

void NsSkeleton::sleepWhileActive() try {
	shared_ptr<vector<shared_ptr<NsSkelRpcServer> > > servers = NsSkelRpcRegistry::instance()->servers();
	auto loopWorker = NsSkelRpcRegistry::instance()->getLoopWorker();	
	bool active = true;
	while(active) {		
		if(loopWorker) (*loopWorker)();
		active = false;
		for (auto it = servers->begin(); it != servers->end(); it++) {
			active = active || it->get()->active();
		}
		this_thread::sleep_for(1ms);
	}
} catch (NsException &ex) {
	stringstream ess;
	ess << "NsSkeleton::sleepWhileActive(): NsException: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (std::exception &ex) {
	stringstream ess;
	ess << "NsSkeleton::sleepWhileActive(): std::exception: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (...) {
	throw NsException(NSE_POSITION, "NsSkeleton::sleepWhileActive(): Unexpected failure");
}

void NsSkeleton::registerLoopWorker(std::shared_ptr<NsSkelLoopWorkerInterface> worker) try {
	NsSkelRpcRegistry::instance()->enableLoopWorker(worker);
} catch (NsException &ex) {
	stringstream ess;
	ess << "NsSkeleton::registerLoopWorker(): NsException: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (std::exception &ex) {
	stringstream ess;
	ess << "NsSkeleton::registerLoopWorker(): std::exception: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (...) {
	throw NsException(NSE_POSITION, "NsSkeleton::registerLoopWorker(): Unexpected failure");
}

void NsSkeleton::unregisterLoopWorker() try {
	NsSkelRpcRegistry::instance()->disableLoopWorker();
} catch (NsException &ex) {
	stringstream ess;
	ess << "NsSkeleton::unregisterLoopWorker(): NsException: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (std::exception &ex) {
	stringstream ess;
	ess << "NsSkeleton::unregisterLoopWorker(): std::exception: " << ex.what();
	throw NsException(NSE_POSITION, ess);
} catch (...) {
	throw NsException(NSE_POSITION, "NsSkeleton::unregisterLoopWorker(): Unexpected failure");
}
