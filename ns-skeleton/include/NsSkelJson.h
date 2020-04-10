/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   NsSkelJson.h
 * Author: dmerkushov
 *
 * Created on May 25, 2019, 2:33 PM
 */

#ifndef NSSKELJSON_H
#define NSSKELJSON_H

#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <vector>
#include <exception>

#include <cstdint>

#include "NsException.h"

namespace nanoservices {

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonPrettyfier
//
////////////////////////////////////////////////////////////////////////////////

/**
 * JSON prettyfier class. Used to supply indents and other non-meaningful
 * elements to a stream when pretty-printing JSON.
 */
	class NsSkelJsonPrettyfier {
	private:
		/**
		 * Indentation symbols per 1 indentation stop
		 */
		std::string _indent;

		/**
		 * Space between, i.e., a key, a semicolon and a value in a JSON object
		 */
		std::string _space;
	public:

		NsSkelJsonPrettyfier();

		/**
		 * If pretty-printing, insert necessary indents to the std::string stream. If not
		 * pretty-printing, do nothing
		 * @param prettyPrint
		 * @param ss
		 * @param depth
		 */
		void indent(bool prettyPrint, std::stringstream &ss, uint32_t depth);

		/**
		 * If pretty-printing, insert necessary spaces to the std::string stream. If not
		 * pretty-printing, do nothing
		 * @param prettyPrint
		 * @param ss
		 */
		void space(bool prettyPrint, std::stringstream &ss);

		/**
		 * Set indentation symbols per 1 indentation stop
		 * @param newIndent
		 */
		void setIndent(const char *newIndent);

		/**
		 * Set space symbols per 1 space
		 * @param newSpace
		 */
		void setSpace(const char *newSpace);

		/**
		 * Get the global instance of NsSkelJsonPrettyfier
		 * @return
		 */
		static NsSkelJsonPrettyfier &instance();
	};

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonValueType
//
////////////////////////////////////////////////////////////////////////////////

/**
 * JSON value types enumerated
 */
	enum NsSkelJsonValueType {
		JSON_NULL = 0,
		JSON_OBJECT = 1,
		JSON_ARRAY = 2,
		JSON_STRING = 3,
		JSON_NUMBER = 4,
		JSON_BOOLEAN = 5
	};

/**
 * Get a symbolic name of a JSON type
 * @param type
 * @return
 */
	std::string verboseNsSkelJsonType(const NsSkelJsonValueType type);

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonValueBase
//
////////////////////////////////////////////////////////////////////////////////

/**
 * Base class for all JSON classes
 */
	class NsSkelJsonValueBase {
	private:
		NsSkelJsonValueType _type;

	protected:

		NsSkelJsonValueBase(NsSkelJsonValueType type);

	public:

		/**
		 * Get the type of the contained JSON value
		 * @return
		 */
		NsSkelJsonValueType type() const;

		/**
		 * Serialize the contained JSON value as a well-formed JSON
		 * @param prettyPrint
		 * @param depth
		 * @return
		 */
		virtual std::string serialize(bool prettyPrint = false, uint32_t depth = 1) = 0;
	};

/**
 * Shared pointer to any JSON value
 */
	typedef std::shared_ptr<NsSkelJsonValueBase> NsSkelJsonPtr;

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonNull
//
////////////////////////////////////////////////////////////////////////////////

/**
 * JSON null
 */
	class NsSkelJsonNull : public NsSkelJsonValueBase {
	public:

		NsSkelJsonNull();

	public:

		std::string serialize(bool prettyPrint = false, uint32_t depth = 1) override;
	};

/**
 * Shared pointer to a JSON null value
 */
	typedef std::shared_ptr<NsSkelJsonNull> NsSkelJsonNullPtr;

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonString
//
////////////////////////////////////////////////////////////////////////////////

/**
 * String
 */
	class NsSkelJsonString : public NsSkelJsonValueBase, public std::string {
	public:

		NsSkelJsonString();

		NsSkelJsonString(std::string initial);

		std::string serialize(bool prettyPrint = false, uint32_t depth = 1);
	};

/**
 * Shared pointer to a JSON std::string value
 */
	typedef std::shared_ptr<NsSkelJsonString> NsSkelJsonStringPtr;

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonObject
//
////////////////////////////////////////////////////////////////////////////////

/**
 * JSON object
 */
	class NsSkelJsonObject : public NsSkelJsonValueBase, public std::map<std::string, NsSkelJsonPtr> {
	public:

		NsSkelJsonObject();

		NsSkelJsonObject(std::map<std::string, NsSkelJsonPtr> &base);

	public:

		std::string serialize(bool prettyPrint = false, uint32_t depth = 1) override;

		/**
		 * Add the contents of another JSON object to this one. If another object's
		 * key already exists in this instance, the corresponding key-value pair is
		 * kept unchanged in this
		 *
		 * @param another
		 */
		void add(const NsSkelJsonObject &another);
	};

/**
 * Shared pointer to a JSON object value
 */
	typedef std::shared_ptr<NsSkelJsonObject> NsSkelJsonObjectPtr;

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonArray
//
////////////////////////////////////////////////////////////////////////////////

/**
 * JSON array
 */
	class NsSkelJsonArray : public NsSkelJsonValueBase, public std::vector<NsSkelJsonPtr> {
	public:

		NsSkelJsonArray();

		NsSkelJsonArray(std::vector<NsSkelJsonPtr> &base);

		std::string serialize(bool prettyPrint = false, uint32_t depth = 1);

		/**
		 * Add the contents of another JSON array to the end of this one
		 *
		 * @param another
		 */
		void add(const NsSkelJsonArray &another);
	};

/**
 * Shared pointer to a JSON array value
 */
	typedef std::shared_ptr<NsSkelJsonArray> NsSkelJsonArrayPtr;

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonNumber
//
////////////////////////////////////////////////////////////////////////////////

/**
 * JSON number, backed by a double value
 */
	class NsSkelJsonNumber : public NsSkelJsonValueBase {
	private:

		double _value;

	public:

		NsSkelJsonNumber(double v = 0.0);

		double value();

		std::string serialize(bool prettyPrint = false, uint32_t depth = 1);
	};

/**
 * Shared pointer to a JSON number value
 */
	typedef std::shared_ptr<NsSkelJsonNumber> NsSkelJsonNumberPtr;

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonBoolean
//
////////////////////////////////////////////////////////////////////////////////

/**
 * JSON boolean
 */
	class NsSkelJsonBoolean : public NsSkelJsonValueBase {
	private:

		bool _value;

	public:

		NsSkelJsonBoolean(bool v = true);

		bool value();

		std::string serialize(bool prettyPrint = false, uint32_t depth = 1);
	};

/**
 * Shared pointer to a JSON boolean value
 */
	typedef std::shared_ptr<NsSkelJsonBoolean> NsSkelJsonBooleanPtr;

////////////////////////////////////////////////////////////////////////////////
//
// JSON pointer casts
//
////////////////////////////////////////////////////////////////////////////////

/**
 * Cast a NsSkelJsonPtr to a typed NsSkelJson...Ptr class.
 *
 * Throws a NsException if r is empty, or if the requested destination type is
 * not corresponding with the instance contained in r.
 *
 * @param r must not be empty
 * @return
 */
	template<class T>
	T castNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		if (!p) {
			throw NsException(NSE_POSITION, "Cannot cast from an empty NsSkelJsonPtr");
		}

		std::stringstream ess;
		ess << "Requested target type is " << typeid(T).name() << ", but it is not supported by castNsSkelJsonPtr()";
		throw (NsException(NSE_POSITION, ess));
	}

	template<>
	NsSkelJsonNullPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonObjectPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonArrayPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonStringPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonNumberPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonBooleanPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<typename T>
	T fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		if (!p) {
			throw NsException(NSE_POSITION, "Empty NsSkelJsonPtr provided");
		}

		std::stringstream ess;
		ess << "Requested target type is " << typeid(T).name() << ", but it is not supported by fromJsonValue()";
		throw (NsException(NSE_POSITION, ess));
	}

	template<>
	NsSkelJsonObject fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	std::map<std::string, NsSkelJsonPtr> fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonArray fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	std::vector<NsSkelJsonPtr> fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonString fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	std::string fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonNumber fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	double fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	int64_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	uint64_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	int32_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	uint32_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	int16_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	uint16_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	int8_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	uint8_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonBoolean fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	bool fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

	template<>
	NsSkelJsonNull fromNsSkelJsonPtr(const NsSkelJsonPtr &p);

/**
 * Set the value pointed by p to v
 * @param p
 * @param v
 */
	template<typename T>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, T &v) {
		if (typeid(T) == typeid(NsSkelJsonValueBase)) {
			std::stringstream ess;
			ess << "Cannot create a NsSkelJsonPtr to NsSkelJsonValueBase";
			throw (NsException(NSE_POSITION, ess));
		} else {
			std::stringstream ess;
			ess << "Requested type is " << typeid(T).name() << ", but it is not supported by setNsSkelJsonPtr()";
			throw (NsException(NSE_POSITION, ess));
		}
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonObject &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, std::map<std::string, NsSkelJsonPtr> &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonArray &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, std::vector<NsSkelJsonPtr> &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonString &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, std::string &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonNumber &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, double &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, int64_t &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, uint64_t &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, int32_t &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, uint32_t &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, int16_t &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, uint16_t &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, int8_t &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, uint8_t &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonBoolean &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, bool &v);

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonNull &v);

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonParser
//
////////////////////////////////////////////////////////////////////////////////

/**
 * JSON parser
 */
	class NsSkelJsonParser {
	private:

		void skipSpaces(std::istream &is, char &c);

		NsSkelJsonPtr parseNull(std::istream &is, char &c);

		NsSkelJsonPtr parseObject(std::istream &is, char &c);

		NsSkelJsonPtr parseArray(std::istream &is, char &c);

		NsSkelJsonPtr parseString(std::istream &is, char &c);

		NsSkelJsonPtr parseNumber(std::istream &is, char &c);

		NsSkelJsonPtr parseBoolean(std::istream &is, char &c);

		NsSkelJsonPtr parseBasedOnFirst(std::istream &is, char &c);

	public:

		NsSkelJsonParser();

		/**
		 * Parse a JSON from an input stream.
		 *
		 * Before parsing:
		 * the stream has to be opened;
		 * the stream position has to be before or on the first character of JSON entity to parse.
		 *
		 * Upon successful parsing:
		 * the stream is not closed;
		 * the stream position will be after the last character of the parsed JSON entity, so if several JSON entities are contained sequentially in the stream, the method may be called several times.
		 *
		 * Throws NsException in case of parsing errors. In case of exception, the stream position, as well as the state of the stream (opened/closed/bad/etc), is undefined.
		 *
		 * @param is
		 * @return NsSkelJsonPtr to the parsed entity representation
		 */
		NsSkelJsonPtr parse(std::istream &is);

		/**
		 * Parse a JSON from a std::string.
		 *
		 * Throws NsException in case of parsing errors.
		 *
		 * @param s
		 * @return
		 */
		NsSkelJsonPtr parse(std::string &s);

		/**
		 * Parse a JSON from an input stream, then convert it to the given NsSkelJson...Ptr type.
		 *
		 * Before parsing:
		 * the stream has to be opened;
		 * the stream position has to be before or on the first character of JSON entity to parse.
		 *
		 * Upon successful parsing:
		 * the stream is not closed;
		 * the stream position will be after the last character of the parsed JSON entity, so if several JSON entities are contained sequentially in the stream, the method may be called several times.
		 *
		 * Throws NsException in case of parsing errors, or wrong type argument. In case of exception, the stream position, as well as the state of the stream (opened/closed/bad/etc), is undefined.
		 *
		 * @param is
		 * @return NsSkelJsonPtr to the parsed entity representation
		 */
		template<class T>
		T typedParse(std::istream &is) {
			NsSkelJsonPtr jp = parse(is);
			return castNsSkelJsonPtr<T>(jp);
		}

		/**
		 * Parse a JSON from a std::string, then convert it to the given NsSkelJson...Ptr type.
		 *
		 * Throws NsException in case of parsing errors, or wrong type argument.
		 *
		 * @param s
		 * @return
		 */
		template<class T>
		T typedParse(std::string &s) {
			std::stringstream ss;
			ss << s;
			return typedParse<T>(ss);
		}
	};
}

#endif /* NSSKELJSON_H */

