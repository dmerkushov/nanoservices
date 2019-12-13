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
 * File:   NsRpcResponse.cpp
 * Author: dmerkushov
 *
 * Created on May 14, 2019, 3:31 PM
 */

#include "NsRpcResponse.h"

using namespace nanoservices;

NsRpcResponse::NsRpcResponse () {
}

NsRpcResponse::NsRpcResponse (const NsRpcResponse& orig) {
}

NsRpcResponse::~NsRpcResponse () {
}

bool NsRpcResponse::success () {
	return _success;
}

void NsRpcResponse::setSuccess (bool success) {
	_success = success;
}

NsSerialized & NsRpcResponse::result () {
	return _result;
}

void NsRpcResponse::setResult (NsSerialized &result) {
	_result = result;
}
