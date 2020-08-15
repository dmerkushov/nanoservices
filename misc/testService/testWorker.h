#pragma once

#include <NsSkelLoopWorkerInterface.h>

class TestWorker : public nanoservices::NsSkelLoopWorkerInterface {
public:
	TestWorker(int timeout) : NsSkelLoopWorkerInterface(timeout) {};
	void process() override;
};
