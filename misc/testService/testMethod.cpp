#include "testMethod.h"

std::shared_ptr<TestMethodResult> TestMethod::processRequest(std::shared_ptr<TestMethodArgs> args) {
	auto res = std::make_shared<TestMethodResult>();
	res->c = args->a + args->b;
	return res;
}
