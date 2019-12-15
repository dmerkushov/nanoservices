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
	TEST_START_DESCRIBE("Regression test on issue https://github.com/dmerkushov/nanoservices/issues/3\n\n"
						"1. A sends a request to B\n"
						"2. B starts processing the request, the processing is time-consuming\n"
						"3. A dies\n"
						"4. B finishes processing, tries to send a response, but falls into an infinite loop in writeBin()");

	testPassed();
}
