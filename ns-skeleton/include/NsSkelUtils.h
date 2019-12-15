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
 * File:   NsSkelUtils.h
 * Author: dmerkushov
 *
 * Created on May 29, 2019, 1:02 PM
 */

#ifndef NSSKELUTILS_H
#define NSSKELUTILS_H

#include <string>
#include <sstream>
#include <iosfwd>
#include <memory>

#include "exchange/logging/LogArgs.h"
#include "NsSerializer.h"

typedef std::stringstream logss;

namespace nanoservices {

	class NsSkelUtils {
	public:
		NsSkelUtils();

		NsSkelUtils(const NsSkelUtils &orig);

		virtual ~NsSkelUtils();

		static void log(LogLevel level, std::string &record);

		static void log(LogLevel level, std::ostream &record);

		static std::shared_ptr<std::string> toBase64(std::shared_ptr<NsSerialized> data);

		static std::shared_ptr<NsSerialized> fromBase64(std::shared_ptr<std::string> base64);

	private:
	};
}

#endif /* NSSKELUTILS_H */

