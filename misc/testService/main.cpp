#include <NsSkeleton.h>
#include "testMethod.h"
#include "testWorker.h"

using namespace std;
using namespace nanoservices;

int main(int argc, char** argv) {
	NsSkeleton::init("testService", argc, argv);

	NsSkeleton::registerReplier(static_pointer_cast<NsSkelRpcReplierInterface, TestMethod>(make_shared<TestMethod>()));
	NsSkeleton::registerLoopWorker(static_pointer_cast<NsSkelLoopWorkerInterface, TestWorker>(make_shared<TestWorker>(50000)));
	NsSkeleton::startup();
	NsSkeleton::sleepWhileActive();
	NsSkeleton::shutdown();
	return 0;
}
