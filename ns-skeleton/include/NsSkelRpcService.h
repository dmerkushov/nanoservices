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
 * File:   NsSkelRpcService.h
 * Author: dmerkushov
 *
 * Created on March 19, 2019, 6:25 PM
 */

#ifndef NSSKELRPCSERVICE_H
#define NSSKELRPCSERVICE_H

#include <cstdint>
#include <memory>
#include <string>

#include "NsSkelJson.h"
#include "NsException.h"

namespace nanoservices {

	class NsSkelRpcService {
	public:
		NsSkelRpcService(std::shared_ptr<std::string> serviceName, std::shared_ptr<std::string> host, uint16_t port,
						 bool httpEnabled, uint16_t httpPort);

		NsSkelRpcService(std::shared_ptr<std::string> serviceName, NsSkelJsonPtr serviceJson) throw(NsException);

		virtual ~NsSkelRpcService();

		std::shared_ptr<std::string> serviceName();

		std::shared_ptr<std::string> host();

		uint16_t port();

		bool httpEnabled();

		uint16_t httpPort();

	private:
		NsSkelRpcService(const NsSkelRpcService &orig) = delete;

		void operator=(const NsSkelRpcService &orig) = delete;

		std::shared_ptr<std::string> _serviceName;
		std::shared_ptr<std::string> _host;
		uint16_t _port;
		bool _httpEnabled;
		uint16_t _httpPort;
	};
}

#endif /* NSSKELRPCSERVICE_H */

