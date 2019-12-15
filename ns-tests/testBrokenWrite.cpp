//
// Created by dmerkushov on 12/15/19.
//

#include "testing.h"

#include "testBrokenWrite.h"

#include "NsMonitoring.h"
#include "NsSkeleton.h"
#include "NsSerializer.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelConfiguration.h"
#include "NsSkelRpcServer.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelRpcRegistry.h"
#include "NsSkelUtils.h"

#include <string>

using namespace std;
using namespace nanoservices;

void testBrokenWrite() {
	TEST_START;

	string serviceName = "myService";
	NsSkeleton::init(serviceName);

	cout << "Skeleton configured" << endl;

	shared_ptr<NsSkelRpcReplierInterface> replier(new MyReplier());

	NsSkeleton::registerReplier(replier);
}
