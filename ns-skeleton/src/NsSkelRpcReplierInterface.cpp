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
 * File:   NsSkelRpcReplierBase.cpp
 * Author: dmerkushov
 *
 * Created on May 15, 2019, 6:06 PM
 */

#include "NsSkelRpcReplierInterface.h"

using namespace nanoservices;

NsSkelRpcReplierInterface::NsSkelRpcReplierInterface() {
}

NsSkelRpcReplierInterface::~NsSkelRpcReplierInterface() {
}

bool NsSkelRpcReplierInterface::isThreadsafe() throw(NsException) {
	return false;
}
