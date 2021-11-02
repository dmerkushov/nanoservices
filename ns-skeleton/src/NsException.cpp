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
 * File:   NsException.cpp
 * Author: dmerkushov
 *
 * Created on February 21, 2019, 6:39 PM
 */

#include "NsException.h"

#include <sstream>
#include <typeinfo>
#include <cstdio>
#include <cstring>
#include <execinfo.h>

#define NOPOSDESCRIPTION "No position description provided"

using namespace nanoservices;
using namespace std;

NsException::NsException(string cause) :
		exception(),
		_shortDescription(cause),
		_stacktrace(NOPOSDESCRIPTION) {
	init();
}

NsException::NsException(string position, string cause) :
		exception(),
		_shortDescription(cause),
		_stacktrace(position) {
	init();
}

NsException::NsException(string position, stringstream &cause) :
		exception(),
		_shortDescription(cause.str()),
		_stacktrace(position) {
	init();
}

NsException::NsException(stringstream &cause) :
		exception(),
		_shortDescription(cause.str()),
		_stacktrace(NOPOSDESCRIPTION) {
	init();
}

NsException::NsException(string position, string cause, const NsException &rootEx) :
		exception(),
		_shortDescription(cause),
		_stacktrace(position) {
	init(rootEx.stacktrace(), rootEx.shortDescription(), rootEx.fullDescription());
}

NsException::NsException(string position, stringstream &cause, const NsException &rootEx) :
		exception(),
		_shortDescription(cause.str()),
		_stacktrace(position) {
	init(rootEx.stacktrace(), rootEx.shortDescription(), rootEx.fullDescription());
}

NsException::NsException(string position, const NsException &rootEx) :
		exception(),
		_shortDescription(rootEx.shortDescription()),
		_stacktrace(position) {
	init(rootEx.stacktrace(), rootEx.shortDescription(), rootEx.fullDescription());
}

NsException::NsException(const NsException &rootEx) :
		exception(),
		_shortDescription(rootEx.shortDescription()),
		_stacktrace(NOPOSDESCRIPTION) {
	init(rootEx.stacktrace(), rootEx.shortDescription(), rootEx.fullDescription());
}

NsException::NsException(const string &position, const stringstream &shortDescription, const string &rootStacktrace,
						 const string &rootShortDescription, const string &rootFullDescription) :
		exception(),
		_shortDescription(shortDescription.str()),
		_stacktrace(position) {
	init(rootStacktrace, rootShortDescription, rootFullDescription);
}

NsException::~NsException() noexcept {
}

const char *NsException::what() const noexcept {
	return _fullDescription.c_str();
}

const string &NsException::stacktrace() const noexcept {
	return _stacktrace;
}

const string &NsException::shortDescription() const noexcept {
	return _shortDescription;
}

const string &NsException::fullDescription() const noexcept {
	return _fullDescription;
}

const string &NsException::rootExceptionFullDescription() const noexcept {
	return _rootExceptionFullDescription;
}

void NsException::init(const string &rootExStack, const string &rootExShort, const string &rootExFull) {
	void **addressArr = new void *[NSE_STACKTRACE_SIZE_MAX];
	size_t stacktraceSize = backtrace(addressArr, NSE_STACKTRACE_SIZE_MAX);
	char **stacktraceArr = backtrace_symbols(addressArr, stacktraceSize);

	std::stringstream ss;
	ss << _stacktrace << " - ";
	if (stacktraceSize >= 3) {
		for (unsigned int i = 2; i < stacktraceSize; i++) {
			ss << stacktraceArr[i] << endl;
		}
	}

	_stacktrace = ss.str();

	free(stacktraceArr);

	delete[] addressArr;

	stringstream wss;
	try {
		wss << typeid(*this).name();
	} catch (std::bad_typeid &bti) {
		wss << "(Unknown exception class)";
	}
	wss << " : " << _shortDescription << endl;
	wss << _stacktrace;

	if (rootExFull != "") {
		wss << endl << endl;
		wss << "\tCaused by:" << endl;
		wss << rootExFull;
	}

	_fullDescription = wss.str();

	_rootExceptionFullDescription = rootExFull;
}
