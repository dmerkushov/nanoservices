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

#include "testBase64.h"

#include <string>

#include "testing.h"

#include "NsSerializer.h"
#include "NsSkelUtils.h"

using namespace std;
using namespace nanoservices;

void nanoservices::tests::testBase64::testBase64() {
	TEST_START_DESCRIBE("Test Base64 encoding/decoding routines\n"
						"by encoding/decoding sample lines\n"
						"that had used to cause problems");

	string ololo = "/home/databin/Maps/Podolsk/Podolsk.map";
	for (
			int i = 0; i < 4; i++
			) {
		string ololi = ololo.substr(0, ololo.size() - i);
		shared_ptr<NsSerialized> ololiSer = make_shared<NsSerialized>(ololi.c_str(), ololi.size());
		shared_ptr<string> base64s = NsSkelUtils::toBase64(ololiSer);
		shared_ptr<NsSerialized> ololiDeser = NsSkelUtils::fromBase64(base64s);
		string ololiDeserS(ololiDeser->ptr, ololiDeser->size);
		cout << ololi << "\t-> " << *base64s << "\t-> " << ololiDeserS << endl;

		if (ololi != ololiDeserS) {
			testFailed();
			return;
		}
	}

	ololo = "/home/databin/Maps/N-37-017/N-37-017.MAP";
	for (
			int i = 0; i < 4; i++
			) {
		string ololi = ololo.substr(0, ololo.size() - i);
		shared_ptr<NsSerialized> ololiSer = make_shared<NsSerialized>(ololi.c_str(), ololi.size());
		shared_ptr<string> base64s = NsSkelUtils::toBase64(ololiSer);
		shared_ptr<NsSerialized> ololiDeser = NsSkelUtils::fromBase64(base64s);
		string ololiDeserS(ololiDeser->ptr, ololiDeser->size);
		cout << ololi << "\t-> " << *base64s << "\t-> " << ololiDeserS << endl;

		if (ololi != ololiDeserS) {
			testFailed();
			return;
		}
	}

	testPassed();
}