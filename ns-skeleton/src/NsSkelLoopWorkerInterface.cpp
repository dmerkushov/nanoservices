#include "NsSkelLoopWorkerInterface.h"

using namespace std;
using namespace nanoservices;

NsSkelLoopWorkerInterface::NsSkelLoopWorkerInterface(int timeout): _timeout(timeout), _work(true) {
}

void NsSkelLoopWorkerInterface::disable() {
	_work = false;
}

void NsSkelLoopWorkerInterface::operator() () {
	if(!_work) return;
	auto tp = std::chrono::high_resolution_clock::now();
	if(tp - _pp > _timeout) {
		try {
			process();
		} catch(...) {
		}
		_pp = std::chrono::high_resolution_clock::now();
	}
}
