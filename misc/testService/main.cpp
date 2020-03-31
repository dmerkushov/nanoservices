#include <NsSkeleton.h>

using namespace std;
using namespace nanoservices;

int main(int argc, char** argv) {
	NsSkeleton::init("testService", argc, argv);

	NsSkeleton::startup();
	NsSkeleton::sleepWhileActive();
	NsSkeleton::shutdown();
	return 0;
}
