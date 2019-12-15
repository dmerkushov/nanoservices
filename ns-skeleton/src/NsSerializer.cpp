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
 * File:   SerializedData.cpp
 * Author: dmerkushov
 *
 * Created on May 15, 2019, 12:29 PM
 */

#include "NsSerializer.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace nanoservices;

NsBinaryData::NsBinaryData() : msgpack2::type::raw_ref() {
}

NsBinaryData::NsBinaryData(const char *p, uint32_t s) : msgpack2::type::raw_ref(p, s) {
	// DEBUG
	//	cout << "NsBinaryData const-char constructor" << endl;
	//	cout << "\tusing:" << endl << hexdump (p, s) << endl;
}

NsBinaryData::NsBinaryData(const NsBinaryData &orig) {
	this->ptr = new char[orig.size];
	this->size = orig.size;
	::memset(const_cast<char *> (this->ptr), 0, this->size);

	// DEBUG
	//	cout << "NsBinaryData copy constructor" << endl;
	//	cout << "\tcopying:" << endl << hexdump (orig.ptr, orig.size) << endl;

	char *p = const_cast<char *> (this->ptr);
	::memcpy(p, orig.ptr, this->size);

	// DEBUG
	//	cout << "\tto:" << endl << hexdump (this->ptr, this->size) << endl;

	setOwnMemory();
}

void NsBinaryData::operator=(const NsBinaryData &orig) {
	if (ownMemory()) {
		delete[] this->ptr;
	}

	this->ptr = new char[orig.size];
	this->size = orig.size;
	::memset(const_cast<char *> (this->ptr), 0, this->size);

	// DEBUG
	//	cout << "NsBinaryData operator=" << endl;
	//	cout << "\tcopying:" << endl << hexdump (orig.ptr, orig.size) << endl;

	char *p = const_cast<char *> (this->ptr);
	::memcpy(p, orig.ptr, size);

	// DEBUG
	//	cout << "\tto:" << endl << hexdump (this->ptr, this->size) << endl;

	setOwnMemory();
}

bool NsBinaryData::ownMemory() {
	return _ownMemory;
}

void NsBinaryData::setOwnMemory(bool ownMemory) {
	_ownMemory = ownMemory;
}

NsBinaryData::~NsBinaryData() {
	if (_ownMemory) {
		delete[] this->ptr;
	}
}

void NsBinaryData::msgpack_unpack(const msgpack2::object &o) {
	// DEBUG
	//	cout << "NsBinaryData msgpack_unpack" << endl;


	if (o.type != msgpack2::type::BIN) {
		throw msgpack2::type_error();
	}
	msgpack2::object_bin objbin = o.via.bin;

	// DEBUG
	//	cout << "\tcopying:" << endl << hexdump (objbin.ptr, objbin.size) << endl;

	this->ptr = new char[objbin.size];
	this->size = objbin.size;
	char *p = const_cast<char *> (this->ptr);
	::memcpy(p, objbin.ptr, size);

	// DEBUG
	//	cout << "\tto:" << endl << hexdump (this->ptr, this->size) << endl;

	_ownMemory = true;
}

NsBinBuffer::NsBinBuffer() : msgpack2::sbuffer() {
}

NsBinBuffer::NsBinBuffer(uint32_t size) : msgpack2::sbuffer(size) {
}

NsBinBuffer::NsBinBuffer(const msgpack2::type::raw_ref &bin) {
	this->write(bin.ptr, bin.size);
}

NsBinBuffer::~NsBinBuffer() {
}

std::shared_ptr<NsBinaryData> NsBinBuffer::toPackable() {
	int s = this->size();
	char *d = this->release();

	//	cout << "NsBinBuffer::toSerializable::copying from address " << hex << "0x" << ((int64_t) d) << dec << ", size " << s << endl;

	NsBinaryData res(d, s); // Make use of the non-copy constructor
	std::shared_ptr<NsBinaryData> result = make_shared<NsBinaryData>(res); // Make use of the copy constructor

	::free(d);

	//	cout << "NsBinBuffer::toSerializable::copied to address " << hex << "0x" << ((int64_t) result->ptr) << dec << ", size " << result->size << endl;

	return result;
}

string nanoservices::hexdump(const char *data, uint32_t len) {
	stringstream hds;

	string ds(data, len);
	stringstream dss(ds);

	unsigned long address = 0;
	hds << hex << setfill('0');
	while (1) {
		char buf[16];
		int nread;

		for (nread = 0; nread < 16 && dss.get(buf[nread]); nread++);
		if (nread == 0) break;

		hds << setw(8) << address;

		for (int i = 0; i < 16; i++) {
			if (i % 8 == 0) hds << ' ';
			if (i < nread)
				hds << ' ' << setw(2) << (((unsigned) buf[i]) & 0xFF);
			else
				hds << "   ";
		}

		hds << "   ";
		for (int i = 0; i < nread; i++) {
			if (buf[i] < 32)
				hds << '.';
			else
				hds << buf[i];
		}

		hds << endl;
		address += 16;
	}

	string hd;
	hd = hds.str();

	return hd;
}
