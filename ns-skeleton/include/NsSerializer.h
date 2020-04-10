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

#include <cstdint>
#include <cstring>

#include "NsException.h"

#include <msgpack.hpp>

namespace msgpack2 = msgpack::v2;

#define NSSERIALIZER_PREPARE MSGPACK_DEFINE
#define NSSERIALIZER_ADD_ENUM MSGPACK_ADD_ENUM

namespace nanoservices {

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

		std::shared_ptr<NsSerialized> serialize(P &toSerialize) try {
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

		std::shared_ptr<NsSerialized> serialize(std::shared_ptr<P> toSerialize) try {
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

		std::shared_ptr<P> deserialize(NsSerialized &toDeserialize) try {
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

		std::shared_ptr<P> deserialize(std::shared_ptr<NsSerialized> toDeserialize) try {
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

