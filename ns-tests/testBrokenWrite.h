//
// Created by dmerkushov on 12/15/19.
//

#ifndef TESTBROKENWRITE_H
#define TESTBROKENWRITE_H

#include <unistd.h>

#include "exchange/NsVoidArgs.h"
#include "exchange/NsVoidResult.h"
#include "NsMonitoring.h"
#include "NsSkeleton.h"
#include "NsSerializer.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelConfiguration.h"
#include "NsSkelRpcServer.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelRpcRegistry.h"
#include "NsSkelUtils.h"

namespace nanoservices {
namespace tests {
namespace testBrokenWrite {

class TestBrokenWrite_A_Args : public NsVoidArgs {
};

class TestBrokenWrite_A_Result : public NsVoidResult {
};

class TestBrokenWrite_B_Args {
public:

	TestBrokenWrite_B_Args() {
	}

	TestBrokenWrite_B_Args(TestBrokenWrite_B_Args &orig) {
		this->i = orig.i;
	}

	virtual ~TestBrokenWrite_B_Args() {
	}

	uint32_t i;

	NSSERIALIZER_PREPARE (i);
};

class TestBrokenWrite_B_Result {
public:

	TestBrokenWrite_B_Result() {
	}

	TestBrokenWrite_B_Result(TestBrokenWrite_B_Result &orig) {
		this->o = orig.o;
	}

	virtual ~TestBrokenWrite_B_Result() {
	}

	uint32_t o;

	NSSERIALIZER_PREPARE (o);
};

class TestBrokenWrite_A_Replier
		: public nanoservices::NsSkelRpcReplier<TestBrokenWrite_A_Args, TestBrokenWrite_A_Result> {
public:

	TestBrokenWrite_A_Replier() {
	}

	virtual ~TestBrokenWrite_A_Replier() {
	}

	virtual std::shared_ptr<std::string> methodName() noexcept(true) {
		return _methodName;
	}

	virtual std::shared_ptr<TestBrokenWrite_A_Result>
	processRequest(std::shared_ptr<TestBrokenWrite_A_Args> args) {

		std::cout << "Replier A: preparing a call to B" << std::endl;

		std::shared_ptr<TestBrokenWrite_B_Args> argsB = std::make_shared<TestBrokenWrite_B_Args>();

		nanoservices::NsSkeleton::call<TestBrokenWrite_B_Args, TestBrokenWrite_B_Result>(
				"myService",
				"repB",
				argsB,
				false
		);

		int sleepSec = 5;
		std::cout << "Replier A: now sleeping for " << sleepSec << " seconds" << std::endl;
		::sleep(sleepSec);

		std::cout << "Replier A: returning result" << std::endl;
		std::shared_ptr<TestBrokenWrite_A_Result> result = std::make_shared<TestBrokenWrite_A_Result>();
		return result;
	}

private:
	std::shared_ptr<std::string> _methodName = std::make_shared<std::string>("repA");
};

class TestBrokenWrite_B_Replier
		: public nanoservices::NsSkelRpcReplier<TestBrokenWrite_B_Args, TestBrokenWrite_B_Result> {
public:

	TestBrokenWrite_B_Replier() {
	}

	virtual ~TestBrokenWrite_B_Replier() {
	}

	virtual std::shared_ptr<std::string> methodName() noexcept(true) {
		return _methodName;
	}

	virtual std::shared_ptr<TestBrokenWrite_B_Result>
	processRequest(std::shared_ptr<TestBrokenWrite_B_Args> args) {

		std::cout << "Replier B: args: i=" << args->i << std::endl;

		std::shared_ptr<TestBrokenWrite_B_Result> result = std::make_shared<TestBrokenWrite_B_Result>();

		result->o = args->i + 1;

		int sleepSec = 5;
		std::cout << "Replier B: now sleeping for " << sleepSec << " seconds" << std::endl;
		::sleep(sleepSec);

		std::cout << "Replier B: returning result " << result->o << std::endl;
		return result;
	}

private:
	std::shared_ptr<std::string> _methodName = std::make_shared<std::string>("repB");
};

void testBrokenWrite();
}
}
}

#endif //TESTBROKENWRITE_H
