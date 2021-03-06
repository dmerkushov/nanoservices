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
 * File:   NsSkelRpcReplierRegistry.h
 * Author: dmerkushov
 *
 * Created on March 12, 2019, 2:23 PM
 */

#ifndef NSSKELRPCREPLIERREGISTRY_H
#define NSSKELRPCREPLIERREGISTRY_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "NsException.h"
#include "NsSkelRpcService.h"
#include "NsSkelRpcReplierInterface.h"
#include "NsSkelLoopWorkerInterface.h"

namespace nanoservices {

	class NsSkelRpcServer;

	class NsSkelRpcRegistry {
	public:
		static std::shared_ptr<NsSkelRpcRegistry> instance();

		virtual ~NsSkelRpcRegistry();

		std::shared_ptr<NsSkelRpcReplierInterface> getReplier(std::shared_ptr<std::string> method);

		void registerReplier(std::shared_ptr<NsSkelRpcReplierInterface> replier);

		void unregisterReplier(std::shared_ptr<std::string> methodName);

		void enableLoopWorker(std::shared_ptr<NsSkelLoopWorkerInterface> worker);

		std::shared_ptr<NsSkelLoopWorkerInterface> getLoopWorker();

		void disableLoopWorker();

		std::shared_ptr<std::vector<std::string> > methods();

		std::shared_ptr<NsSkelRpcService> getService(std::shared_ptr<std::string> serviceName);

		std::shared_ptr<NsSkelRpcService> getLocalService();

		void registerServer(std::shared_ptr<NsSkelRpcServer> server);

		void unregisterServer(std::shared_ptr<NsSkelRpcServer> server);

		void startupServers();

		void shutdownServers();

		std::shared_ptr<std::vector<std::shared_ptr<NsSkelRpcServer> > > servers();

		void initialize();

	private:
		NsSkelRpcRegistry();

		NsSkelRpcRegistry(const NsSkelRpcRegistry &orig) = delete;

		void operator=(const NsSkelRpcRegistry &orig) = delete;

		void prepareServicesMap();

		std::map<std::string, std::shared_ptr<NsSkelRpcReplierInterface> > _repliers;

		std::map<std::string, std::shared_ptr<NsSkelRpcService> > _services;
		bool _servicesMapReady = false;

		std::vector<std::shared_ptr<NsSkelRpcServer> > _servers;

		std::shared_ptr<NsSkelLoopWorkerInterface> _loopWorker = nullptr;
	};
}

// There is a cyclic dependency between NsSkelRpcServer and NsSkelRpcRegistry
// TODO: Move registry of servers from NsSkelRpcRegistry to another place to break the cyclic dependency
#include "NsSkelRpcServer.h"

#endif /* NSSKELRPCREPLIERREGISTRY_H */

