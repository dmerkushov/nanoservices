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
 * File:   NsRpcResponseError.h
 * Author: dmerkushov
 *
 * Created on May 14, 2019, 4:06 PM
 */

#ifndef NSRPCRESPONSEERROR_H
#define NSRPCRESPONSEERROR_H

#include <exception>
#include <map>
#include <string>

#include <cstdint>

#include "NsSerializer.h"

namespace nanoservices {

	class NsRpcResponseError {
	public:
		NSSERIALIZER_PREPARE_SYS (_errorCode, _errorDescription, _optionalErrorParams);

		NsRpcResponseError();

		NsRpcResponseError(int64_t errorCode, const char *errorDescription);

		NsRpcResponseError(int64_t errorCode, std::string &errorDescription);

		NsRpcResponseError(int64_t errorCode, std::string &errorDescription,
						   std::map<std::string, std::string> &optionalErrorParams);

		NsRpcResponseError(int64_t errorCode, std::string prefix, NsException &exception);

		NsRpcResponseError(int64_t errorCode, std::string prefix, std::exception &exception);

		NsRpcResponseError(const NsRpcResponseError &orig);

		virtual ~NsRpcResponseError();

		int64_t errorCode();

		std::string &errorDescription();

		std::map<std::string, std::string> &optionalErrorParams();

	private:
		int64_t _errorCode;
		std::string _errorDescription;
		std::map<std::string, std::string> _optionalErrorParams;
	};
}

#endif /* NSRPCRESPONSEERROR_H */

