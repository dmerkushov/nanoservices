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
 * File:   testExceptionThrowThrough.h
 * Author: dmerkushov
 *
 * Created on October 23, 2019, 4:36 PM
 */

#ifndef TESTEXCEPTIONTHROWTHROUGH_H
#define TESTEXCEPTIONTHROWTHROUGH_H

#include <unistd.h>

#include "NsMonitoring.h"
#include "NsSkeleton.h"
#include "NsSerializer.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelConfiguration.h"
#include "NsSkelRpcServer.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelRpcRegistry.h"
#include "NsSkelUtils.h"

class MyArgs {
public:

	MyArgs() {
		::memset(k, 0, sizeof(k));
	}

	MyArgs(MyArgs &orig) {
		this->i = orig.i;
		this->j = orig.j;
		::memset(k, 0, sizeof(k));
	}

	virtual ~MyArgs() {
	}

	uint32_t i;
	uint32_t j;

	uint32_t k[102400];

	NSSERIALIZER_PREPARE (i, j, k
	);
};

class MyResult {
public:

	MyResult() {
	}

	MyResult(MyResult &orig) {
		this->o = orig.o;
	}

	virtual ~MyResult() {
	}

	uint32_t o;
	uint32_t a = 0;

	NSSERIALIZER_PREPARE (o, a
	);
};

class MyReplier : public nanoservices::NsSkelRpcReplier<MyArgs, MyResult> {
public:

	MyReplier() {
	}

	virtual ~MyReplier() {
	}

	virtual std::shared_ptr <std::string> methodName() throw(nanoservices::NsException) {
		return _methodName;
	}

	virtual std::shared_ptr <MyResult> processRequest(std::shared_ptr <MyArgs> args) throw(nanoservices::NsException) {
		throw nanoservices::NsException(NSE_POSITION, "Exception from MyReplier:processRequest");

		std::cout << "Replier: params: i=" << args->i << ", j=" << args->j << std::endl;

		//		if (params->i >= 10000) {
		//			throw (NsException (NSE_POSITION, "i is too big"));
		//		}

		std::shared_ptr <MyResult> result = std::make_shared<MyResult>();

		//		try {
		//			sendRpcRequest<MyParam, MyResult> (std::make_shared<std::string> ("sfgafs"), std::make_shared<std::string> ("skjghg"), params, true);
		//		} catch (NsSkelRpcException ex) {
		//			std::stringstream ess;
		//			ess << "NsSkelRpcException: " << ex.what ();
		//			throw NsSkelRpcException (NSE_POSITION, ess);
		//		}

		result->o = args->i + 1;

		return result;
	}

private:
	std::shared_ptr <std::string> _methodName = std::make_shared<std::string>("doSmth");
};

void testExceptionThrowThrough();

#endif /* TESTEXCEPTIONTHROWTHROUGH_H */

