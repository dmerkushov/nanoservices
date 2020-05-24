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
 * File:   NsSkelRpcReplierBase.h
 * Author: dmerkushov
 *
 * Created on May 15, 2019, 6:06 PM
 */

#ifndef NSSKELRPCREPLIERBASE_H
#define NSSKELRPCREPLIERBASE_H

#include <string>
#include <memory>

#include "NsSerializer.h"
#include "NsException.h"

namespace nanoservices {

	class NsSkelRpcReplierInterface {
	public:
		NsSkelRpcReplierInterface();

		virtual ~NsSkelRpcReplierInterface();

		virtual std::shared_ptr<std::string> methodName() noexcept(true) = 0;

		virtual bool isThreadsafe() noexcept(true);

		virtual std::shared_ptr<NsSerialized>
		processSerializedRequest(std::shared_ptr<NsSerialized> args) = 0;

		virtual std::shared_ptr<NsSerialized> processSerializedRequest(NsSerialized &args) = 0;

	private:
		NsSkelRpcReplierInterface(const NsSkelRpcReplierInterface &orig) = delete;

		void operator=(const NsSkelRpcReplierInterface &orig) = delete;
	};
}

#endif /* NSSKELRPCREPLIERBASE_H */

