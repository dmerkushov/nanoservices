/*
 * Copyright 2019 dmerkushov.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "testing.h"
#include "testExceptionThrowThrough.h"

#include <atomic>
#include <iostream>
#include <memory>
#include <string>

#include <cstdint>

#include <unistd.h>

#include "NsMonitoring.h"
#include "NsSkeleton.h"
#include "NsSerializer.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelConfiguration.h"
#include "NsSkelRpcServer.h"
#include "NsSkelRpcReplier.h"
#include "NsSkelRpcRegistry.h"
#include "NsSkelUtils.h"

using namespace std;
using namespace nanoservices;
using namespace nanoservices::tests::testExceptionThrowThrough;

void nanoservices::tests::testExceptionThrowThrough::testExceptionThrowThrough() {
	TEST_START;

	string serviceName = "myService";
	NsSkeleton::init(serviceName);

	cout << "Skeleton configured" << endl;

	shared_ptr<NsSkelRpcReplierInterface> replier(new TestExceeptionThrowThrough_Replier());

	NsSkeleton::registerReplier(replier);

	cout << "Replier registered" << endl;

	NsSkelJsonArray knownServices = NsSkelConfiguration::instance()->getParameter<NsSkelJsonArray>("known-services");

	cout << "Known services: " << knownServices.serialize() << endl;

	NsSkeleton::startup();

	sleep(1);

	shared_ptr<TestExceeptionThrowThrough_Args> params = make_shared<TestExceeptionThrowThrough_Args>();
	//	//	MyParam *params = new MyParam ();
	params->i = 0;
	params->j = 0;

	//
	//	////////////////////////////////////////////////////////////////////////////
	//	msgpack2::sbuffer sbuf;
	//	msgpack2::pack (sbuf, *params);
	//
	//	uint32_t size = sbuf.size ();
	//
	//	cout << hex;
	//
	//	for (int i = 0; i <= size; i++) {
	//		cout << " " << setw (2) << (unsigned) (sbuf.data ()[i]);
	//	}
	//
	//	cout << endl;
	//
	//	msgpack2::object_handle oh = msgpack2::unpack (sbuf.data (), sbuf.size ());
	//	msgpack2::object obj = oh.get ();
	//
	//	std::shared_ptr<MyParam> prepared = make_shared<MyParam> ();
	//	obj.convert (*prepared);
	//
	//	cout << dec;
	//	cout << "deser i: " << prepared->i << endl;
	//	cout << "deser j: " << prepared->j << endl;

	////////////////////////////////////////////////////////////////////////////

	string sName = "myService";
	string mName = "doSmth";
	int times = 1;
	NsSerializer<TestExceptionThrowThrough_Result> myResultSerializer;
	for (
			int i = 0; i < times; i++
			) {
		bool e = false;

		cout << "Before sending request" << endl;

		shared_ptr<TestExceptionThrowThrough_Result> result;
		try {
			result = NsSkeleton::call<TestExceeptionThrowThrough_Args, TestExceptionThrowThrough_Result>(
					"myService", mName, params, true);
		} catch (NsException &ex) {
			e = true;
			cerr << "myService called threw an exception: " << ex.shortDescription() << endl;
		}

		if (!e) {
			cout << "result " << i << ": " << result->o << endl;
		}
	}

	//	sleep (10);
	//
	//	cout << "10...";
	//
	//	sleep (10);
	//
	//	cout << "20...";
	//
	//	sleep (10);
	//
	//	cout << "30...";
	//
	//	sleep (10);
	//
	//	cout << "40" << endl;

	NsSkeleton::shutdown();



	////////////////////////////////////////////////////////////////////////////
	//
	//	uint8_t ui = 12;
	//
	//	NsMonitoringArray nma;
	//	nma.add<string> ("blablabla");
	//	nma.add<int32_t> (1653453);
	//	nma.add<uint8_t> (127);
	//
	//	::timespec ts;
	//	::clock_gettime (CLOCK_REALTIME, &ts);
	//
	//	uint64_t millisOnStart = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
	//
	//	for (int i = 0; i < 1000; i++) {
	//		NsMonitoring::send (NsMonIp4Address ("127.0.0.1"), (uint16_t) 12, ui, (string) "ololo", 65535, nma);
	//		cout << i << endl;
	//		if (i % 10 == 0) {
	//			cout.flush ();
	//		}
	//	}
	//
	//	::clock_gettime (CLOCK_REALTIME, &ts);
	//
	//	uint64_t millisOnFinish = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
	//
	//	cout.flush ();
	//	cerr.flush ();
	//
	//	cout << "Millis on start: " << hex << millisOnStart << ", on finish " << millisOnFinish << dec << endl;
	//	cout << "Total execution time: " << (millisOnFinish - millisOnStart) << endl;

	testPassed();
}