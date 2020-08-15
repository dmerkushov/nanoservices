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
 * File:   NsSkeleton.h
 * Author: dmerkushov
 *
 * Created on June 21, 2019, 4:03 PM
 */

#ifndef NSSKELETON_H
#define NSSKELETON_H

#include <memory>
#include <string>

// DEBUG
#include <iostream>

#include <cstdint>

#include "NsException.h"
#include "NsSkelRpcReplierInterface.h"
#include "NsSkelRpcServer.h"

namespace nanoservices {

	class NsSkeleton {
	public:

		/**
		 * Initialize NsSkeleton infrastructure for a given service name
		 * @param serviceName
		 */
		static void init(const std::string &serviceName, int argc = 0, char **argv = nullptr);

		/**
		 * Start the previously-initialized nanoservice
		 */
		static void startup();

		/**
		 * Shut down the previously-initialized nanoservice
		 */
		static void shutdown();

		/**
		 * Get the current service name
		 * @return
		 */
		static std::shared_ptr<std::string> serviceName();

		/**
		 * Sleep while the current nanoservice is active
		 */
		static void sleepWhileActive();

		/**
		 * Register a replier for a nanoservice method
		 */
		static void registerReplier(std::shared_ptr<NsSkelRpcReplierInterface> replier);

		/**
		 * Unregister the replier previously registered for a nanoservice method
		 */
		static void unregisterReplier(std::shared_ptr<std::string> methodName);

		/**
		 * Register a worker for a nanoservice inner loop
		 */
		static void registerLoopWorker(std::shared_ptr<NsSkelLoopWorkerInterface> worker);

		/**
		 * Unregister the worker previously registered for a nanoservice inner loop
		 */
		static void unregisterLoopWorker();

		template<typename Args, typename Result>
		static std::shared_ptr<Result>
		call(
				std::shared_ptr<std::string> serviceName,
				std::shared_ptr<std::string> methodName,
				std::shared_ptr<Args> args,
				bool waitForResponse
		) {

			return nanoservices::sendRpcRequest<Args, Result>(serviceName, methodName, args, waitForResponse);
		}

		template<typename Args, typename Result>
		static std::shared_ptr<Result>
		call(
				const std::string &serviceName,
				const std::string &methodName,
				std::shared_ptr<Args> args,
				bool waitForResponse
		) {

			std::shared_ptr<std::string> serviceNamePtr = std::make_shared<std::string>(serviceName);
			std::shared_ptr<std::string> methodNamePtr = std::make_shared<std::string>(methodName);

			return nanoservices::sendRpcRequest<Args, Result>(serviceNamePtr, methodNamePtr, args, waitForResponse);
		}

		template<typename Args, typename Result>
		static std::shared_ptr<Result> call(
				const char *serviceName,
				const char *methodName,
				std::shared_ptr<Args> args,
				bool waitForResponse
		) {

			std::shared_ptr<std::string> serviceNamePtr = std::make_shared<std::string>(serviceName);
			std::shared_ptr<std::string> methodNamePtr = std::make_shared<std::string>(methodName);

			return nanoservices::sendRpcRequest<Args, Result>(serviceNamePtr, methodNamePtr, args, waitForResponse);
		}
	};
}

#endif /* NSSKELETON_H */

