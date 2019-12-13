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
 * File:   NsRpcResponseError.cpp
 * Author: dmerkushov
 *
 * Created on May 14, 2019, 4:06 PM
 */

#include "NsRpcResponseError.h"

#include <string>

using namespace std;
using namespace nanoservices;

NsRpcResponseError::NsRpcResponseError () :
_errorCode (0),
_errorDescription ("") {
}

NsRpcResponseError::NsRpcResponseError (int64_t errorCode, const char* errorDescription) :
_errorCode (errorCode),
_errorDescription (errorDescription) {
}

NsRpcResponseError::NsRpcResponseError (int64_t errorCode, string &errorDescription) :
_errorCode (errorCode),
_errorDescription (errorDescription) {
}

NsRpcResponseError::NsRpcResponseError (int64_t errorCode, string &errorDescription, map<string, string> &optionalErrorParams) :
_errorCode (errorCode),
_errorDescription (errorDescription),
_optionalErrorParams (optionalErrorParams) {
}

NsRpcResponseError::NsRpcResponseError (int64_t errorCode, string prefix, NsException &exception) :
_errorCode (errorCode) {
	stringstream ess;
	ess << prefix << exception.what ();
	_errorDescription += ess.str ();
}

NsRpcResponseError::NsRpcResponseError (int64_t errorCode, string prefix, std::exception &exception) :
_errorCode (errorCode) {
	stringstream ess;
	ess << prefix << exception.what ();
	_errorDescription += ess.str ();
}

NsRpcResponseError::NsRpcResponseError (const NsRpcResponseError& orig) :
_errorCode (orig._errorCode),
_errorDescription (orig._errorDescription),
_optionalErrorParams (orig._optionalErrorParams) {
}

NsRpcResponseError::~NsRpcResponseError () {
}

int64_t NsRpcResponseError::errorCode () {
	return _errorCode;
}

string & NsRpcResponseError::errorDescription () {
	return _errorDescription;
}

map<string, string> & NsRpcResponseError::optionalErrorParams () {
	return _optionalErrorParams;
}
