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
 * File:   NsSkelRpcReplier.h
 * Author: dmerkushov
 *
 * Created on February 28, 2019, 2:55 PM
 */

#ifndef NSSKELRPCREPLIER_H
#define NSSKELRPCREPLIER_H

#include <memory>
#include <string>

#include "NsException.h"
#include "NsException.h"
#include "NsSerializer.h"
#include "NsRpcResponseError.h"
#include "NsSkelRpcReplierInterface.h"

namespace nanoservices {

	template<typename Args, typename Result>
	class NsSkelRpcReplier : public NsSkelRpcReplierInterface {
	public:

		NsSkelRpcReplier() {
		}

		virtual ~NsSkelRpcReplier() {
		}

		std::shared_ptr<NsSerialized>
		processSerializedRequest(std::shared_ptr<NsSerialized> sArgs) override try {
			std::shared_ptr<Args> args = _argsSerializer.deserialize(sArgs);
			std::shared_ptr<Result> result = processRequest(args);
			std::shared_ptr<NsSerialized> sResult = _resultSerializer.serialize(result);
			return sResult;
		} catch (NsException &e) {
			throw NsException(NSE_POSITION, e.what());
		} catch (std::exception &e) {
			throw NsException(NSE_POSITION, e.what());
		} catch (...) {
			throw NsException(NSE_POSITION, "processSerializedRequest(): Unexpected failure");
		}

		std::shared_ptr<NsSerialized> processSerializedRequest(NsSerialized &sArgs) override try {
			std::shared_ptr<Args> args = _argsSerializer.deserialize(sArgs);
			std::shared_ptr<Result> result = processRequest(args);
			std::shared_ptr<NsSerialized> sResult = _resultSerializer.serialize(result);
			return sResult;
		} catch (NsException &e) {
			throw NsException(NSE_POSITION, e.what());
		} catch (std::exception &e) {
			throw NsException(NSE_POSITION, e.what());
		} catch (...) {
			throw NsException(NSE_POSITION, "processSerializedRequest(): Unexpected failure");
		}

		virtual std::shared_ptr<Result> processRequest(std::shared_ptr<Args> args) = 0;

		NsTypeInfoPtr getArgsType() {return _types.first; };

		NsTypeInfoPtr getReturnType() {return _types.second; };

	private:
		NsSkelRpcReplier(const NsSkelRpcReplier &orig) = delete;

		void operator=(const NsSkelRpcReplier &orig) = delete;

		NsSerializer<Args> _argsSerializer;
		NsSerializer<Result> _resultSerializer;
		std::pair<NsTypeInfoPtr, NsTypeInfoPtr> _types = std::make_pair<NsTypeInfoPtr, NsTypeInfoPtr>(getFullTypeName<Args>(), getFullTypeName<Result>());; 
	};
}

#endif /* NSSKELRPCREPLIER_H */

