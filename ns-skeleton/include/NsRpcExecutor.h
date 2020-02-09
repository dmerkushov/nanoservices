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
 * File:   NsRpcExecutor.h
 * Author: dmerkushov
 *
 * Created on June 19, 2019, 5:03 PM
 */

#ifndef NSRPCEXECUTOR_H
#define NSRPCEXECUTOR_H

#include <memory>
#include <string>

#include "NsSerializer.h"
#include "NsException.h"
#include "NsRpcResponse.h"

namespace nanoservices {

std::shared_ptr<NsSerialized>
processRpcRequest(std::shared_ptr<NsSerialized> rpcRequestSerialized, bool &waitForResponse) noexcept;
}

#endif /* NSRPCEXECUTOR_H */

