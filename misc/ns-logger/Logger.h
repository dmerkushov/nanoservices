#pragma once

#include <exchange/logging/LogArgs.h>
#include <exchange/NsVoidResult.h>
#include <NsSkelRpcReplier.h>
#include <NsSkelLoopWorkerInterface.h>

class Logger : public nanoservices::NsSkelRpcReplier<LogArgs, NsVoidResult>, public nanoservices::NsSkelLoopWorkerInterface {
public:
	Logger(int timeout) : NsSkelLoopWorkerInterface(timeout) {};
	std::shared_ptr<NsVoidResult> processRequest(std::shared_ptr<LogArgs>);
	std::shared_ptr<std::string> methodName() noexcept(true) override {
		return std::make_shared<std::string>("log");
	};
	void process() override;
private:
	std::deque<std::shared_ptr<LogArgs>> _queue;
};
