#pragma once

#include "NsSkelRpcReplier.h"

class TestMethodArgs {
public:
	int32_t a;
	int32_t b;
	std::vector<std::string> descs;
	NSSERIALIZER_PREPARE(a,b, descs);
};

class TestMethodResult {
public:
	int32_t c;
	NSSERIALIZER_PREPARE(c);
};

class TestMethod: public nanoservices::NsSkelRpcReplier<TestMethodArgs, TestMethodResult> {
public:
	std::shared_ptr<TestMethodResult> processRequest(std::shared_ptr<TestMethodArgs> args) throw(nanoservices::NsException);
	std::shared_ptr<std::string> methodName() throw(nanoservices::NsException) {
		return std::make_shared<std::string>(std::string("test-method"));
	};
};
