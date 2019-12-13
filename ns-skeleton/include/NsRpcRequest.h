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
 * File:   NsRpcRequest.h
 * Author: dmerkushov
 *
 * Created on May 14, 2019, 3:15 PM
 */

#ifndef NSRPCREQUEST_H
#define NSRPCREQUEST_H

#include <string>

#include "NsSerializer.h"

namespace nanoservices {

class NsRpcRequest {
public:
	NSSERIALIZER_PREPARE (_method, _argsSerialized, _waitForResponse);

	NsRpcRequest ();
	NsRpcRequest (std::string &method, NsSerialized &argsSerialized, bool waitForResponse = true);
	virtual ~NsRpcRequest ();

	std::string &method ();
	NsSerialized &argsSerialized ();
	bool waitForResponse ();
private:
	std::string _method;
	NsSerialized _argsSerialized;
	bool _waitForResponse;
};

}

#endif /* NSRPCREQUEST_H */

