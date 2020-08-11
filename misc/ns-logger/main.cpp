#include <iostream>

#include <NsSkeleton.h>

#include "Logger.h"

using namespace std;
using namespace nanoservices;

int main(int argc, char** argv) {
	NsSkeleton::init("ns-logger", argc, argv);

	auto logger = make_shared<Logger>(10);
	NsSkeleton::registerReplier(static_pointer_cast<NsSkelRpcReplierInterface, Logger>(logger));
	NsSkeleton::registerLoopWorker(static_pointer_cast<NsSkelLoopWorkerInterface, Logger>(logger));
	NsSkeleton::startup();
	NsSkeleton::sleepWhileActive();
	NsSkeleton::shutdown();
	return 0;
}
