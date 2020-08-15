#include <iostream>
#include <NsSkeleton.h>

#include "testWorker.h"

void TestWorker::process() {
	static int cnt = 0;
	std::cerr << "IN loop: " << cnt++ << std::endl;
	if(cnt > 50) nanoservices::NsSkeleton::unregisterLoopWorker();
}
