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

/*
 * File:   NsSkelUtils.cpp
 * Author: dmerkushov
 *
 * Created on May 29, 2019, 1:02 PM
 */

#include "NsSkelUtils.h"

#include <iostream>
#include <string>
#include <sstream>

#include <chrono>

#include <ctype.h>

#include "NsSkelRpcServer.h"
#include "NsSkelRpcRegistry.h"

#include "exchange/NsVoidResult.h"
#include "exchange/logging/LogArgs.h"

#include <cstdlib>

using namespace std;
using namespace nanoservices;

NsSkelUtils::NsSkelUtils() {
}

NsSkelUtils::NsSkelUtils(const NsSkelUtils &orig) {
}

NsSkelUtils::~NsSkelUtils() {
}

////////////////////////////////////////////////////////////////////////////////
//
// Logging utils
//
////////////////////////////////////////////////////////////////////////////////

void NsSkelUtils::log(LogLevel level, ostream &record) {
	stringstream ss;
	ss << record.rdbuf();

	string recordStr = ss.str();

	log(level, recordStr);
}

static shared_ptr<string> loggingServiceName = make_shared<string>("ns-logger");
static shared_ptr<string> loggingLogMethodName = make_shared<string>("log");

string getTimestamp() {
  // get a precise timestamp as a string
  const auto now = std::chrono::system_clock::now();
  const auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);
  const auto nowMs = std::chrono::duration_cast<std::chrono::microseconds>(
      now.time_since_epoch()) % 1000000;
  std::stringstream nowSs;
  nowSs
      << std::put_time(std::localtime(&nowAsTimeT), "%a %b %d %Y %T")
      << '.' << std::setfill('0') << std::setw(6) << nowMs.count();
  return nowSs.str();
}

void NsSkelUtils::log(LogLevel level, string &record) {
	static bool inLog = false;
	shared_ptr<LogArgs> args = make_shared<LogArgs>();
	if(getenv("NS_NO_LOG") != nullptr) return;
	if(inLog) return;
	args->logLevel = level;
	args->sourceService = *(NsSkelRpcRegistry::instance()->getLocalService()->serviceName());
	args->text = record;
	args->time = getTimestamp();

	try {
		inLog = true;
		sendRpcRequest<LogArgs, NsVoidResult>(loggingServiceName, loggingLogMethodName, args, false);
		inLog = false;
	} catch (NsException &ex) {
		cerr << "==============================" << endl
			 << "WARNING: Could not write a log record to the logging service:" << endl
			 << record << endl;
		cerr << "because of an exception (NsException): " << ex.what() << endl
			 << "==============================" << endl;
	} catch (std::exception &ex) {
		cerr << "==============================" << endl
			 << "WARNING: Could not write a log record to the logging service:" << endl
			 << record << endl;
		cerr << "because of an exception (std::exception): " << ex.what() << endl
			 << "==============================" << endl;
	} catch (...) {
		cerr << "==============================" << endl
			 << "WARNING: Could not write a log record to the logging service:" << endl
			 << record << endl;
		cerr << "because of an non-catchable exception" << endl
			 << "==============================" << endl;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Base64 utils
//
////////////////////////////////////////////////////////////////////////////////

namespace nanoservices {
	static const std::string base64_chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
	static const char base64_term = '=';
}

static inline bool isBase64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

static inline void base64_ca3_to_ca4(unsigned char *ca3, unsigned char *ca4) {
	ca4[0] = (ca3[0] & 0xFC) >> 2;
	ca4[1] = ((ca3[0] & 0x03) << 4) + ((ca3[1] & 0xF0) >> 4);
	ca4[2] = ((ca3[1] & 0x0F) << 2) + ((ca3[2] & 0xC0) >> 6);
	ca4[3] = ca3[2] & 0x3F;
}

shared_ptr<string> NsSkelUtils::toBase64(std::shared_ptr<NsSerialized> data) {
	if (!data || data->size == 0) {
		shared_ptr<string> ret = make_shared<string>("");
		return ret;
	}

	uint32_t dataSize = data->size;

	stringstream retss;
	unsigned char ca3[3];
	unsigned char ca4[4];
	int i = 0;
	int j = 0;

	char *bytesToEncode = const_cast<char *> (data->ptr);

	while (dataSize--) {
		ca3[i++] = *(bytesToEncode++);
		if (i == 3) {
			base64_ca3_to_ca4(ca3, ca4);

			for (i = 0; i < 4; i++) {
				retss << base64_chars[ca4[i]];
			}
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++) {
			ca3[j] = '\0';
		}

		base64_ca3_to_ca4(ca3, ca4);

		for (j = 0; j < i + 1; j++) {
			retss << base64_chars[ca4[j]];
		}

		while (i++ < 3) {
			retss << base64_term;
		}
	}

	shared_ptr<string> ret = make_shared<string>(retss.str());
	return ret;
}

static inline void base64_ca4_to_ca3(unsigned char *ca4, unsigned char *ca3) {
	ca3[0] = (ca4[0] << 2) + ((ca4[1] & 0x30) >> 4);
	ca3[1] = ((ca4[1] & 0x0F) << 4) + ((ca4[2] & 0x3C) >> 2);
	ca3[2] = ((ca4[2] & 0x03) << 6) + ca4[3];
}

shared_ptr<NsSerialized> NsSkelUtils::fromBase64(shared_ptr<string> base64) {
	size_t base64Size = base64->size();

	int i = 0;
	int j = 0;
	int in_ = 0;

	unsigned char ca4[4];
	unsigned char ca3[3];

	NsBinBuffer b;

	while (base64Size-- && ((*base64)[in_] != base64_term) && isBase64((*base64)[in_])) {
		ca4[i++] = (*base64)[in_];
		in_++;

		if (i == 4) {
			for (i = 0; i < 4; i++) {
				ca4[i] = base64_chars.find(ca4[i]) & 0xFF;
			}

			base64_ca4_to_ca3(ca4, ca3);

			b.write(((const char *) ca3), 3);

			i = 0;
		}
	}

	if (i) {
		for (j = 0; j < i; j++) {
			ca4[j] = base64_chars.find(ca4[j]) & 0xFF;
		}

		base64_ca4_to_ca3(ca4, ca3);

		for (j = 0; j < i - 1; j++) {
			b.write(((const char *) (ca3 + j)), 1);
		}
	}

	return b.toPackable();
}
