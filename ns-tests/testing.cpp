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

#include <iostream>
#include <vector>
#include <string>

using namespace std;

int testsTotalCount = 0;

const char *currentTest;

vector<string> testsPassedNames;
vector<string> testsFailedNames;

std::string replaceAll(std::string str, const std::string &from, const std::string &to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

void startTest(const char *testName, const char *description) {
	cout << "**********************************" << endl;
	cout << "*" << endl;
	cout << "*   Test: " << testName << endl;

	string descr(description);
	if (!descr.empty()) {
		cout << "*" << endl;
		cout << "*   "
			 << replaceAll(descr, "\n", "\n*   ")
			 << endl;
	}

	cout << "*" << endl;

	currentTest = testName;
	testsTotalCount++;
}

void testPassed() {
	cout << "*" << endl;
	cout << "*   ...Test " << currentTest << " passed" << endl;
	cout << "*" << endl;
	cout << "**********************************" << endl;

	testsPassedNames.insert(testsPassedNames.end(), currentTest);
}

void testFailed() {
	cout << "*" << endl;
	cout << "*   ...TEST " << currentTest << " FAILED" << endl;
	cout << "*" << endl;
	cout << "**********************************" << endl;

	testsFailedNames.insert(testsFailedNames.end(), currentTest);
}

void testsResult() {
	cout << "**********************************" << endl;
	cout << "*" << endl;
	cout << "*   Test results:" << endl;
	cout << "*" << endl;
	cout << "*   Total: " << testsTotalCount << endl;
	cout << "*   Passed: " << testsPassedNames.size() << endl;
	cout << "*   Failed: " << testsFailedNames.size() << endl;
	cout << "*" << endl;

	if (testsPassedNames.size() > 0) {
		cout << "*   Passed tests:" << endl;
		for (auto it = testsPassedNames.begin(); it != testsPassedNames.end(); it++) {
			cout << "*   " << *it << endl;
		}
		cout << "*" << endl;
	}

	if (testsFailedNames.size() > 0) {
		cout << "*   Failed tests:" << endl;
		for (auto it = testsFailedNames.begin(); it != testsFailedNames.end(); it++) {
			cout << "*   " << *it << endl;
		}
		cout << "*" << endl;
	}

	cout << "**********************************" << endl;
}