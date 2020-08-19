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

#include <unistd.h>
#include <sys/wait.h>

using namespace std;
using namespace nanoservices;
using namespace nanoservices::tests::testBrokenWrite;

// Forward declarations for functions used in this test
bool parent_B(pid_t childPid);

void child_A();

void nanoservices::tests::testBrokenWrite::testBrokenWrite() {
	TEST_START_DESCRIBE("Regression test on issue https://github.com/dmerkushov/nanoservices/issues/3\n\n"
						"1. A sends a request to B\n"
						"2. B starts processing the request, the processing is time-consuming\n"
						"3. A dies\n"
						"4. B finishes processing, tries to send a response, but falls into an infinite loop in writeBin()");

	/*
	 * Forking a child process. Parent will function as the B side. Child will function as the A side.
	 */
	cout << "Forking child (A side) from the parent (B side)" << endl;

	pid_t childPid = ::fork();
	if (childPid < 0) {
		cerr << "Couldn't fork process" << endl;
		testFailed();
		return;
	}

	if (childPid == 0) {
		cout << "Child: i am A" << endl;
		child_A();
		return;
	}

	cout << "Parent: i am B" << endl;
	bool testSuccess = parent_B(childPid);
	if (testSuccess) {
		testPassed();
		return;
	}
	testFailed();
}

bool parent_B(pid_t childPid) {
	string serviceName = "ns-tests/testBrokenWrite/serviceB";
	NsSkeleton::init(serviceName);

	cout << "Parent(B): NsSkeleton configured" << endl;

	cout << "Parent(B): Registering replier B" << endl;
	shared_ptr<NsSkelRpcReplierInterface> replierB(new TestBrokenWrite_B_Replier());
	NsSkeleton::registerReplier(replierB);

	cout << "Parent(B): Replier registered" << endl;

	NsSkelJsonArray knownServices = NsSkelConfiguration::instance()->getParameter<NsSkelJsonArray>("known-services");

	cout << "Parent(B): Known services: " << knownServices.serialize() << endl;

	NsSkeleton::startup();

	cout << "Parent(B): NsSkeleton started" << endl;

	shared_ptr<TestBrokenWrite_B_Args> args = make_shared<TestBrokenWrite_B_Args>();
	args->i = 10;

	cout << "Parent(B): NsSkeleton shutting down" << endl;
	NsSkeleton::shutdown();

	cout << "Parent(B): waiting for child(A) to finish" << endl;
	int childExitStatus;
	::waitpid(childPid, &childExitStatus, 0);
	int childRetValue = WEXITSTATUS(childExitStatus);

	cout << "Parent(B): Child return value: " << childRetValue << endl;

	cerr.flush();
	cout.flush();
	return (childRetValue == 0);
}

void child_A() {
	string serviceName = "ns-tests/testBrokenWrite/serviceA";
	NsSkeleton::init(serviceName);

	cout << "Child(A): NsSkeleton configured" << endl;

	cout << "Child(A): Registering replier B" << endl;
	shared_ptr<NsSkelRpcReplierInterface> replierA(new TestBrokenWrite_A_Replier());
	NsSkeleton::registerReplier(replierA);

	cout << "Child(A): Replier registered" << endl;

	NsSkelJsonArray knownServices = NsSkelConfiguration::instance()->getParameter<NsSkelJsonArray>("known-services");

	cout << "Child(A): Known services: " << knownServices.serialize() << endl;

	NsSkeleton::startup();

	cout << "Child(A): NsSkeleton started" << endl;

	shared_ptr<TestBrokenWrite_B_Args> args = make_shared<TestBrokenWrite_B_Args>();

	cout << "Child(A): NsSkeleton shutting down" << endl;
	NsSkeleton::shutdown();

	cerr.flush();
	cout.flush();
	::exit(0);
}