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
 * File:   SerializedData.h
 * Author: dmerkushov
 *
 * Created on May 15, 2019, 12:29 PM
 */

#ifndef SERIALIZEDDATA_H
#define SERIALIZEDDATA_H

#include <iostream>
#include <iomanip>

#include <memory>
#include <sstream>
#include <numeric>
#include <regex>

#include <cstdint>
#include <cstring>

#include "NsException.h"

#define MSGPACK_USE_CPP03

#include <msgpack.hpp>

namespace msgpack2 = msgpack::v2;

#define NSSERIALIZER_PREPARE_SYS MSGPACK_DEFINE
#define NSSERIALIZER_PREPARE(...) std::vector<std::string> __names__ = nanoservices::ListTypeNames(__VA_ARGS__); \
NSSERIALIZER_PREPARE_SYS(__VA_ARGS__)
#define NSSERIALIZER_ADD_ENUM MSGPACK_ADD_ENUM

namespace nanoservices {

	template<class ...Ts> struct TypeList {};

	template<class T> constexpr std::string helper() {
		return __PRETTY_FUNCTION__;
	};
	
	template<class T> constexpr std::string getTypeName() {
		std::smatch sm;
 
		std::regex re1(R"(\[with T = (.*);)");
		auto search = helper<T>();
		std::regex_search(search, sm, re1);
		return sm.str(1);
	};

	template<typename ...Ts> constexpr std::vector<std::string> ListTypeNames(Ts... args) {
		return {nanoservices::getTypeName<Ts>()...};
	};

	class NsTypeInfo {
		std::string _name;
		std::vector<std::string> _fields;
	public:
		NsTypeInfo(std::string name, std::vector<std::string> fields) {
			_name = name;
			_fields = fields;
		};

		inline std::string getName() {return _name;};
		inline std::string getDefinition() {
			std::string resultText = "// Definition of " + _name + "\n\nclass ";
			resultText += _name;
			resultText += "{\npublic:\n";
			int i = 1;
			std::vector<std::string> fnames;
			for(auto fieldType: _fields) {
				std::string fname = "_" + std::to_string(i++);
				fnames.push_back(fname);
				resultText += "\t" + fieldType + " " + fname + ";\n";
			}
			resultText += "\tNSSERIALIZER_PREPARE(" + std::accumulate(std::next(fnames.begin()), fnames.end(),
                                    fnames[0], // start with first element
                                    [](std::string a, std::string b) {
				         return std::move(a) + ',' + std::move(b);
				     }) + ");\n};";
			return resultText;
		};
	};

	using NsTypeInfoPtr = std::shared_ptr<NsTypeInfo>;

	template<class T> constexpr NsTypeInfoPtr getFullTypeName() {
		auto fnames = (T()).__names__;
		return std::make_shared<NsTypeInfo>(getTypeName<T>(), fnames);
	};

	std::string hexdump(const char *data, uint32_t len);

	class NsBinaryData : public msgpack2::type::raw_ref {
	public:

		NsBinaryData();

		NsBinaryData(const char *p, uint32_t s);

		NsBinaryData(const NsBinaryData &orig);

		virtual ~NsBinaryData();

		void operator=(const NsBinaryData &orig);

		bool ownMemory();

		void setOwnMemory(bool ownMemory = true);

		template<typename Packer>
		void msgpack_pack(Packer &pk) const {
			//DEBUG
			//		std::cout << "NsBinaryData msgpack_pack" << std::endl;

			pk.pack_bin(size);
			pk.pack_bin_body(ptr, size);
		}

		void msgpack_unpack(msgpack2::object const &o);

	private:
		bool _ownMemory = false;
	};

	typedef NsBinaryData NsSerialized;

	class NsBinBuffer : public msgpack2::sbuffer {
	public:
		NsBinBuffer();

		NsBinBuffer(uint32_t s);

		NsBinBuffer(const msgpack2::type::raw_ref &bin);

		virtual ~NsBinBuffer();

		std::shared_ptr<NsBinaryData> toPackable();
	};

	template<class P>
	class NsSerializer {
	public:

		NsSerializer() {
		}

		virtual ~NsSerializer() {
		}

		std::shared_ptr<NsSerialized> serialize(P &toSerialize) throw(NsException) try {
			NsBinBuffer sbuf(0);

			msgpack2::pack(sbuf, toSerialize);

			//DEBUG
			//      uint32_t size = sbuf.size ();
			//		std::cout << std::dec << "Serializing type " << typeid (P).name () << " (size " << size << "):" << std::endl;
			//		std::cout << hexdump (sbuf.data (), size);
			//		std::cout << std::endl;

			std::shared_ptr<NsSerialized> serialized = sbuf.toPackable();

			return serialized;
		} catch (...) {
			std::stringstream ess;
			ess << "Msgpack failed to pack object of type " << typeid(P).name();
			throw NsException(NSE_POSITION, ess);
		}

		std::shared_ptr<NsSerialized> serialize(std::shared_ptr<P> toSerialize) throw(NsException) try {
			NsBinBuffer sbuf(0);

			msgpack2::pack(sbuf, *toSerialize);

			//DEBUG
			//      uint32_t size = sbuf.size ();
			//		std::cout << std::dec << "Serializing type " << typeid (P).name () << " (size " << size << "):" << std::endl;
			//		std::cout << hexdump (sbuf.data (), size);
			//		std::cout << std::endl;

			std::shared_ptr<NsSerialized> serialized = sbuf.toPackable();

			return serialized;
		} catch (...) {
			std::stringstream ess;
			ess << "Msgpack failed to pack object of type " << typeid(P).name();
			throw NsException(NSE_POSITION, ess);
		}

		std::shared_ptr<P> deserialize(NsSerialized &toDeserialize) throw(NsException) try {
			//DEBUG
			//		std::cout << std::dec << "Deserializing type " << typeid (P).name () << " (size " << toDeserialize.size << "):" << std::endl;
			//		std::cout << hexdump (toDeserialize.ptr, toDeserialize.size);

			msgpack2::object_handle oh = msgpack2::unpack(toDeserialize.ptr, toDeserialize.size);
			msgpack2::object obj = oh.get();

			//DEBUG
			//		std::cout << "Deserialized object: " << obj << std::endl;

			std::shared_ptr<P> prepared = std::make_shared<P>();
			obj.convert(*prepared);

			return prepared;
		} catch (msgpack2::parse_error &e) {
			std::stringstream ess;
			ess << "Msgpack failed to unpack object of type " << typeid(P).name() << ": parse error: " << e.what();
			throw NsException(NSE_POSITION, ess);
		} catch (msgpack2::unpack_error &e) {
			std::stringstream ess;
			ess << "Msgpack failed to unpack object of type " << typeid(P).name() << ": unpack error: " << e.what();
			throw NsException(NSE_POSITION, ess);
		} catch (msgpack2::type_error &e) {
			std::stringstream ess;
			ess << "Msgpack failed to unpack object of type " << typeid(P).name() << ": type error: " << e.what();
			throw NsException(NSE_POSITION, ess);
		} catch (...) {
			std::stringstream ess;
			ess << "Msgpack failed to unpack object of type " << typeid(P).name() << ": unexpected error";
			throw NsException(NSE_POSITION, ess);
		}

		std::shared_ptr<P> deserialize(std::shared_ptr<NsSerialized> toDeserialize) throw(NsException) try {
			//DEBUG
			//		std::cout << std::dec << "Deserializing type " << typeid (P).name () << " (size " << toDeserialize->size << "):" << std::endl;
			//		std::cout << hexdump (toDeserialize->ptr, toDeserialize->size);

			msgpack2::object_handle oh = msgpack2::unpack(toDeserialize->ptr, toDeserialize->size);
			msgpack2::object obj = oh.get();

			//DEBUG
			//		std::cout << "Deserialized object: " << obj << std::endl;

			std::shared_ptr<P> prepared = std::make_shared<P>();
			obj.convert(*prepared);

			return prepared;
		} catch (msgpack2::parse_error &e) {
			std::stringstream ess;
			ess << "Msgpack failed to unpack object of type " << typeid(P).name() << ": parse error: " << e.what();
			throw NsException(NSE_POSITION, ess);
		} catch (msgpack2::unpack_error &e) {
			std::stringstream ess;
			ess << "Msgpack failed to unpack object of type " << typeid(P).name() << ": unpack error: " << e.what();
			throw NsException(NSE_POSITION, ess);
		} catch (msgpack2::type_error &e) {
			std::stringstream ess;
			ess << "Msgpack failed to unpack object of type " << typeid(P).name() << ": type error: " << e.what();
			throw NsException(NSE_POSITION, ess);
		} catch (...) {
			std::stringstream ess;
			ess << "Msgpack failed to unpack object of type " << typeid(P).name() << ": unexpected error";
			throw NsException(NSE_POSITION, ess);
		}
	};
}

#endif /* SERIALIZEDDATA_H */

