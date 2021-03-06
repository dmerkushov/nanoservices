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
 * File:   NsRpcExecutor.cpp
 * Author: dmerkushov
 *
 * Created on June 19, 2019, 5:03 PM
 */

#include "NsRpcExecutor.h"

#include <mutex>

#include "NsRpcRequest.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelRpcRegistry.h"
#include "NsRpcRequest.h"
#include "NsRpcResponse.h"
#include "NsRpcResponseError.h"
#include "exchange/NsVoidResult.h"

#ifndef RELEASE
#include "NsSkelUtils.h"
#endif

using namespace std;
using namespace nanoservices;

namespace nanoservices {
	extern mutex cout_lock;

	NsSerializer<NsRpcRequest> _requestSerializer;
	NsSerializer<NsRpcResponseError> _errorSerializer;
	NsSerializer<NsRpcResponse> _responseSerializer;
	NsSerializer<NsVoidResult> _voidResultSerializer;
}

////////////////////////////////////////////////////////////////////////////////
//
// Utility functions for processRpcRequest()
//
////////////////////////////////////////////////////////////////////////////////

template<typename T>
shared_ptr<msgpack2::object_handle> packToObj(T &t) {
	msgpack2::sbuffer sbuf;
	msgpack2::pack(sbuf, t);

	return make_shared<msgpack2::object_handle>(msgpack2::unpack(sbuf.data(), sbuf.size()));
}

template<typename ResultType>
std::shared_ptr<NsSerialized> prepareSerializedResponse(std::shared_ptr<ResultType> r, bool success) noexcept try {
	NsSerializer<ResultType> serializer;

	std::shared_ptr<NsSerialized> resultSerialized = serializer.serialize(r);
	std::shared_ptr<NsRpcResponse> response = std::make_shared<NsRpcResponse>();
	response->setSuccess(success);
	response->setResult(*resultSerialized);

	shared_ptr<NsSerialized> responseSerialized = _responseSerializer.serialize(response);
	return responseSerialized;
} catch (NsException &ex) {
	stringstream ess;
	ess << "prepareSerializedResponse(): caught an NsException: " << ex.what();
	NsException ex1(NSE_POSITION, ess);

	std::shared_ptr<NsRpcResponseError> err = std::make_shared<NsRpcResponseError>(-32001, "", ex1);
	shared_ptr<NsSerialized> responseSerialized = prepareSerializedResponse<NsRpcResponseError>(err, false);
	return responseSerialized;
} catch (std::exception &ex) {
	stringstream ess;
	ess << "prepareSerializedResponse(): caught an std::exception: " << ex.what();
	NsException ex1(NSE_POSITION, ess);

	std::shared_ptr<NsRpcResponseError> err = std::make_shared<NsRpcResponseError>(-32001, "", ex1);
	shared_ptr<NsSerialized> responseSerialized = prepareSerializedResponse<NsRpcResponseError>(err, false);
	return responseSerialized;
} catch (...) {
	stringstream ess;
	ess << "prepareSerializedResponse(): unexpected failure";
	NsException ex1(NSE_POSITION, ess);

	std::shared_ptr<NsRpcResponseError> err = std::make_shared<NsRpcResponseError>(-32001, "", ex1);
	shared_ptr<NsSerialized> responseSerialized = prepareSerializedResponse<NsRpcResponseError>(err, false);
	return responseSerialized;
}

////////////////////////////////////////////////////////////////////////////////
//
// processRpcRequest()
//
////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<NsSerialized> nanoservices::processRpcRequest(std::shared_ptr<NsSerialized> rpcRequestSerialized,
															  bool &waitForResponse) noexcept try {

	shared_ptr<NsRpcRequest> request;

	try {
#ifndef RELEASE
		NsSkelUtils::log(LogLevel::Trace, stringstream() << "Start deserialize");
#endif
		request = _requestSerializer.deserialize(rpcRequestSerialized);
#ifndef RELEASE
		NsSkelUtils::log(LogLevel::Trace, stringstream() << "Stop deserialize");
#endif
	} catch (NsException &ex) {
		shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(-32001,
																			 "processRpcRequest(): NsException when deserializing request: ",
																			 ex);
#ifndef RELEASE
		cout_lock.lock();
		cerr << "processRpcRequest(): NsException when deserializing request: " << ex.what() << endl;
		cout_lock.unlock();
#endif

		return prepareSerializedResponse<NsRpcResponseError>(err, false);
	} catch (std::exception &ex) {
		shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(-32001,
																			 "processRpcRequest(): std::exception when deserializing request: ",
																			 ex);

#ifndef RELEASE
		cout_lock.lock();
		cerr << "processRpcRequest(): std::exception when deserializing request: " << ex.what() << endl;
		cout_lock.unlock();
#endif

		return prepareSerializedResponse<NsRpcResponseError>(err, false);
	} catch (...) {
		shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(-32002,
																			 "processRpcRequest(): unexpected failure when deserializing request");

#ifndef RELEASE
		cout_lock.lock();
		cerr << "processRpcRequest(): unexpected failure when deserializing request" << endl;
		cout_lock.unlock();
#endif

		return prepareSerializedResponse<NsRpcResponseError>(err, false);
	}

	waitForResponse = request->waitForResponse();

	shared_ptr<string> methodName = make_shared<string>(request->method());

	NsMonitoring::monitorReceiveRpcRequest(0x807060504030201, methodName); // TODO Generate IDs

#ifndef RELEASE
	// DEBUG
	cout_lock.lock();
	cout << "Server: Incoming request is for method: " << *methodName << endl;
	cout_lock.unlock();
#endif

	if (*methodName == "shutdown") {
		//TODO security with shutting down the server: check the issuer of the shutdown message
		//TODO implement as a replier (the user could then override it)

		NsSkelRpcRegistry::instance()->shutdownServers();

		shared_ptr<NsVoidResult> voidResult = make_shared<NsVoidResult>();
		return prepareSerializedResponse<NsVoidResult>(voidResult, true);
	}

#ifndef RELEASE
	// DEBUG
	cout_lock.lock();
	cout << "Server: Looking for a replier..." << endl;
	cout_lock.unlock();
#endif

	shared_ptr<NsSkelRpcReplierInterface> replier;
	try {
		replier = NsSkelRpcRegistry::instance()->getReplier(methodName);
	} catch (NsException &ex) {
#ifndef RELEASE
		cout_lock.lock();
		cerr << "getReplier() failed for method " << methodName;
		cout_lock.unlock();
#endif

		stringstream ess;
		ess << "processRpcRequest(): getReplier() failed for method " << methodName << ": NsException: " << ex.what();
		NsException ex1(NSE_POSITION, ess);

		shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(-32001, "", ex1);
		return prepareSerializedResponse<NsRpcResponseError>(err, false);
	}

	//TODO Process thread-safe mode
	//	cout_lock.lock ();
	//	cout << "Server: Replier found! Checking its thread safety..." << endl;
	//	cout_lock.unlock ();
	//	bool threadSafe;
	//	try {
	//		threadSafe = replier->isThreadsafe ();
	//	} catch (NsException &ex) {
	//		cout_lock.lock ();
	//		cerr << "isThreadsafe() method of the replier threw an exception: " << ex.what () << endl;
	//		cout_lock.unlock ();
	//
	//		return;
	//	}
	//	cout_lock.lock ();
	//	cout << "Server: Replier found for method: " << *methodName << " (thread-safe: " << threadSafe << ")" << endl;
	//	//DEBUG
	//	std::cout << std::dec << "Server: before deserializing args (size " << request->argsSerialized ().size << "):" << std::hex << std::setfill ('0');
	//	for (uint32_t i = 0; i < request->argsSerialized ().size; i++) {
	//		std::cout << " " << (unsigned) ((request->argsSerialized ().ptr[i]) & 0xFF);
	//	}
	//	std::cout << std::endl << std::dec;
	//	cout_lock.unlock ();

	shared_ptr<NsSerialized> resultSerialized;
	shared_ptr<NsSerialized> errorSerialized;
	shared_ptr<NsRpcResponse> response = make_shared<NsRpcResponse>();

	bool responsePayloadPrepared = true;
	try {
#ifndef RELEASE
		NsSkelUtils::log(LogLevel::Trace, stringstream() << "Start process");
#endif
		resultSerialized = replier->processSerializedRequest(request->argsSerialized());
#ifndef RELEASE
		NsSkelUtils::log(LogLevel::Trace, stringstream() << "Stop process");
#endif

		// DEBUG
		//		cout_lock.lock ();
		//		cout << "Replier success" << endl;
		//		cout_lock.unlock ();

		// DEBUG
		//		std::cout << "processRpcRequest(): resultSerialized: " << std::endl << hexdump (resultSerialized->ptr, resultSerialized->size) << std::endl;

		response->setSuccess(true);
	} catch (NsException &ex) {
		int32_t code = -32000;
		string message(ex.what());
		shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(code, message);
		auto causePair = std::pair<string, string>("cause", ex.shortDescription());
		err->optionalErrorParams().insert(causePair);

#ifndef RELEASE
		cout_lock.lock();
		cerr << "Replier error: " << err->errorCode() << ": " << err->errorDescription() << endl;
		cout_lock.unlock();
#endif

		try {
			errorSerialized = _errorSerializer.serialize(err);
		} catch (NsException &ex1) {
#ifndef RELEASE
			cout_lock.lock();
			cerr << "Could not serialize err object (replier NsException error): " << err->errorCode() << ": "
				 << err->errorDescription() << endl;
			cerr << "Serializer error: " << ex1.what() << endl;
			cout_lock.unlock();
#endif

			responsePayloadPrepared = false;
		} catch (...) {
#ifndef RELEASE
			cout_lock.lock();
			cerr << "Could not serialize err object (replier NsException error): " << err->errorCode() << ": "
				 << err->errorDescription() << endl;
			cerr << "Serializer unexpected error" << endl;
			cout_lock.unlock();
#endif

			responsePayloadPrepared = false;
		}

		response->setSuccess(false);
	} catch (std::exception &ex) {
		int32_t code = -32001;

		stringstream mss;
		mss << "std::exception when trying to reply: " << ex.what();
		string message = mss.str();
		shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(code, message);

		try {
			errorSerialized = _errorSerializer.serialize(err);
		} catch (NsException &ex1) {
#ifndef RELEASE
			cout_lock.lock();
			cerr << "Could not serialize err object (replier std::exception error): " << err->errorCode() << ": "
				 << err->errorDescription() << endl;
			cerr << "Serializer error: " << ex1.what() << endl;
			cout_lock.unlock();
#endif

			responsePayloadPrepared = false;
		} catch (...) {
#ifndef RELEASE
			cout_lock.lock();
			cerr << "Could not serialize err object (replier std::exception error): " << err->errorCode() << ": "
				 << err->errorDescription() << endl;
			cerr << "Serializer unexpected error" << endl;
			cout_lock.unlock();
#endif

			responsePayloadPrepared = false;
		}

#ifndef RELEASE
		cout_lock.lock();
		cerr << "Replier error: " << err->errorCode() << ": " << err->errorDescription() << endl;
		cout_lock.unlock();
#endif

		response->setSuccess(false);
	} catch (...) {
		int32_t code = -32002;

		stringstream mss;
		mss << "Unexpected failure when trying to reply";
		string message = mss.str();
		shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(code, message);

		try {
			errorSerialized = _errorSerializer.serialize(err);
		} catch (NsException &ex1) {
#ifndef RELEASE
			cout_lock.lock();
			cerr << "Could not serialize err object (replier unexpected exception error): " << err->errorCode() << ": "
				 << err->errorDescription() << endl;
			cerr << "Serializer error: " << ex1.what() << endl;
			cout_lock.unlock();
#endif

			responsePayloadPrepared = false;
		} catch (...) {
#ifndef RELEASE
			cout_lock.lock();
			cerr << "Could not serialize err object (replier unexpected exception error): " << err->errorCode() << ": "
				 << err->errorDescription() << endl;
			cerr << "Serializer unexpected error" << endl;
			cout_lock.unlock();
#endif

			responsePayloadPrepared = false;
		}

#ifndef RELEASE
		cout_lock.lock();
		cerr << "Replier error: " << err->errorCode() << ": " << err->errorDescription() << endl;
		cout_lock.unlock();
#endif
		response->setSuccess(false);
	}

	if (!(request->waitForResponse())) {
		shared_ptr<NsVoidResult> voidResult = make_shared<NsVoidResult>();
		return prepareSerializedResponse<NsVoidResult>(voidResult, true);
	}

	if (!responsePayloadPrepared) {
		shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(-32001,
																			 "processRpcRequest(): could not prepare the response payload. See log");
		return prepareSerializedResponse<NsRpcResponseError>(err, false);
	} else {

		if (response->success()) {
			response->setResult(*resultSerialized);
		} else {
			response->setResult(*errorSerialized);
		}

		shared_ptr<NsSerialized> responseSerialized = _responseSerializer.serialize(response);

		// DEBUG
		//		std::cout << "processRpcRequest(): responseSerialized: " << std::endl << hexdump (responseSerialized->ptr, responseSerialized->size) << std::endl;

		return responseSerialized;
	}
} catch (NsException &ex) {
	shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(-32000, "processRpcRequest(): NsException: ",
																		 ex);
	return prepareSerializedResponse<NsRpcResponseError>(err, false);
} catch (std::exception &ex) {
	shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(-32001,
																		 "processRpcRequest(): std::exception: ", ex);
	return prepareSerializedResponse<NsRpcResponseError>(err, false);
} catch (...) {
	shared_ptr<NsRpcResponseError> err = make_shared<NsRpcResponseError>(-32002,
																		 "processRpcRequest(): unexpected failure");
	return prepareSerializedResponse<NsRpcResponseError>(err, false);
}
