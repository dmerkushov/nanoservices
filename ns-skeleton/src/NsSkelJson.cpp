/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "NsSkelJson.h"

using namespace std;
using namespace nanoservices;

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonPrettyfier
//
////////////////////////////////////////////////////////////////////////////////

static NsSkelJsonPrettyfier __prettyfier_instance_dontUseDirectly;

NsSkelJsonPrettyfier &NsSkelJsonPrettyfier::instance() {
	return __prettyfier_instance_dontUseDirectly;
}

NsSkelJsonPrettyfier::NsSkelJsonPrettyfier() : _indent("    "), _space(" ") {
}

void NsSkelJsonPrettyfier::indent(bool prettyPrint, std::stringstream &ss, uint32_t depth) {
	if (prettyPrint) {
		ss << endl;
		for (uint32_t i = 0; i < depth; i++) {
			ss << _indent;
		}
	}
}

void NsSkelJsonPrettyfier::space(bool prettyPrint, std::stringstream &ss) {
	if (prettyPrint) {
		ss << _space;
	}
}

void NsSkelJsonPrettyfier::setIndent(const char *newIndent) {
	_indent = newIndent;
}

void NsSkelJsonPrettyfier::setSpace(const char *newSpace) {
	_space = newSpace;
}

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonValueType
//
////////////////////////////////////////////////////////////////////////////////

std::string nanoservices::verboseNsSkelJsonType(const NsSkelJsonValueType type) {
	switch (type) {
		case JSON_NULL:
			return "null (type 0)";
		case JSON_OBJECT:
			return "object (type 1)";
		case JSON_ARRAY:
			return "array (type 2)";
		case JSON_STRING:
			return "string (type 3)";
		case JSON_NUMBER:
			return "number (type 4)";
		case JSON_BOOLEAN:
			return "boolean (type 5)";
	}

	return "unexpected";
}

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonValueBase
//
////////////////////////////////////////////////////////////////////////////////

NsSkelJsonValueBase::NsSkelJsonValueBase(NsSkelJsonValueType type) : _type(type) {
}

NsSkelJsonValueType NsSkelJsonValueBase::type() const {
	return _type;
}

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonNull
//
////////////////////////////////////////////////////////////////////////////////

NsSkelJsonNull::NsSkelJsonNull() : NsSkelJsonValueBase(NsSkelJsonValueType::JSON_NULL) {
}

std::string NsSkelJsonNull::serialize(bool prettyPrint, uint32_t depth) {
	return "null";
}

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonString
//
////////////////////////////////////////////////////////////////////////////////

NsSkelJsonString::NsSkelJsonString() : NsSkelJsonValueBase(JSON_STRING) {
}

NsSkelJsonString::NsSkelJsonString(std::string initial) : NsSkelJsonValueBase(JSON_STRING), std::string(initial) {
}

std::string NsSkelJsonString::serialize(bool prettyPrint, uint32_t depth) {
	std::stringstream src(*this);
	std::stringstream dest;

	dest << '"';

	char c;
	src.get(c);
	while (!src.eof()) {
		if (c == '\n') {
			dest << "\\n";
		} else if (c == '\r') {
			dest << "\\r";
		} else if (c == '\t') {
			dest << "\\t";
		} else if (c == '"') {
			dest << "\\\"";
		} else {
			dest << c;
		}
		src.get(c);
	}

	dest << '"';

	return dest.str();
}

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonObject
//
////////////////////////////////////////////////////////////////////////////////

NsSkelJsonObject::NsSkelJsonObject() : NsSkelJsonValueBase(JSON_OBJECT) {
}

NsSkelJsonObject::NsSkelJsonObject(std::map<std::string, NsSkelJsonPtr> &base) : NsSkelJsonValueBase(JSON_OBJECT),
																				 map<string, NsSkelJsonPtr>(base) {
}

std::string NsSkelJsonObject::serialize(bool prettyPrint, uint32_t depth) {
	std::stringstream ss;
	ss << "{";

	for (auto it = begin(); it != end();) {
		NsSkelJsonPrettyfier::instance().indent(prettyPrint, ss, depth);

		std::string key = it->first;
		NsSkelJsonString jsonKey(key);

		ss << jsonKey.serialize(prettyPrint, depth); //TODO Special characters in keys
		NsSkelJsonPrettyfier::instance().space(prettyPrint, ss);
		ss << ':';
		NsSkelJsonPrettyfier::instance().space(prettyPrint, ss);

		std::shared_ptr<NsSkelJsonValueBase> val = it->second;
		ss << val->serialize(prettyPrint, depth + 1);

		++it;

		if (it != end()) {
			ss << ",";
		}
	}

	NsSkelJsonPrettyfier::instance().indent(prettyPrint, ss, depth - 1);

	ss << "}";

	return ss.str();
}

void NsSkelJsonObject::add(const NsSkelJsonObject &another) {
	for (auto it = another.begin(); it != another.end(); it++) {
		string key = it->first;
		if (find(key) == end()) {
			(*this)[key] = it->second;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonArray
//
////////////////////////////////////////////////////////////////////////////////

NsSkelJsonArray::NsSkelJsonArray() : NsSkelJsonValueBase(JSON_ARRAY) {
}

NsSkelJsonArray::NsSkelJsonArray(vector<NsSkelJsonPtr> &base) : NsSkelJsonValueBase(JSON_ARRAY),
																vector<NsSkelJsonPtr>(base) {
}

std::string NsSkelJsonArray::serialize(bool prettyPrint, uint32_t depth) {
	std::stringstream ss;
	ss << "[";

	for (auto it = begin(); it != end();) {
		NsSkelJsonPrettyfier::instance().indent(prettyPrint, ss, depth);

		ss << (*it)->serialize(prettyPrint, depth + 1);

		++it;

		if (it != end()) {
			ss << ",";
		}
	}

	NsSkelJsonPrettyfier::instance().indent(prettyPrint, ss, depth - 1);

	ss << "]";

	return ss.str();
}

void NsSkelJsonArray::add(const NsSkelJsonArray &another) {
	for (long unsigned int i = 0; i < another.size(); i++) {
		this->push_back(another[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonNumber
//
////////////////////////////////////////////////////////////////////////////////

NsSkelJsonNumber::NsSkelJsonNumber(double v) : NsSkelJsonValueBase(JSON_NUMBER), _value(v) {
}

double NsSkelJsonNumber::value() {
	return _value;
}

std::string NsSkelJsonNumber::serialize(bool prettyPrint, uint32_t depth) {
	std::stringstream ss;
	ss << _value;
	return ss.str();
}

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonBoolean
//
////////////////////////////////////////////////////////////////////////////////

NsSkelJsonBoolean::NsSkelJsonBoolean(bool v) : NsSkelJsonValueBase(JSON_BOOLEAN), _value(v) {
}

bool NsSkelJsonBoolean::value() {
	return _value;
}

std::string NsSkelJsonBoolean::serialize(bool prettyPrint, uint32_t depth) {
	std::stringstream ss;

	if (_value) {
		ss << "true";
	} else {
		ss << "false";
	}

	return ss.str();
}

////////////////////////////////////////////////////////////////////////////////
//
// NsSkelJsonParser
//
////////////////////////////////////////////////////////////////////////////////

/*
 * The global rule for the parsing methods:
 *
 * When each token is started being parsed:
 * the position on the STREAM is AFTER the first character of the token being parsed
 * char variable 'c' contains the first character of the token
 *
 * When each token is finished being parsed:
 * the position in the stream is AFTER the last character of the token being parsed
 * char variable 'c' contains the last character of the token just parsed
 */

NsSkelJsonParser::NsSkelJsonParser() {
}

NsSkelJsonPtr NsSkelJsonParser::parse(std::string &s) {
	std::stringstream ss;
	ss << s;
	return parse(ss);
}

NsSkelJsonPtr NsSkelJsonParser::parse(istream &is) {
	char c;
	is.get(c);
	skipSpaces(is, c);
	return parseBasedOnFirst(is, c);
}

void NsSkelJsonParser::skipSpaces(istream &is, char &c) {
	//		cout << "skipSpaces" << endl;

	while (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
		if (is.eof()) {
			break;
		}
		if (is.bad()) {
			break;
		}

		c = is.get();
	}
}

NsSkelJsonPtr NsSkelJsonParser::parseBasedOnFirst(istream &is, char &c) {
	//		cout << "parseBasedOnFirst" << endl;

	if (is.eof()) {
		throw (NsException(NSE_POSITION, "Premature end of token being parsed"));
	} else if (c == 'n' || c == 'N') {
		return parseNull(is, c);
	} else if (c == '{') {
		return parseObject(is, c);
	} else if (c == '[') {
		return parseArray(is, c);
	} else if (c == '"') {
		return parseString(is, c);
	} else if (c >= '0' && c <= '9') {
		return parseNumber(is, c);
	} else if (c == 't' || c == 'T' || c == 'f' || c == 'F') {
		return parseBoolean(is, c);
	} else {
		std::stringstream ess;
		ess << "Unrecognized token starting with '" << c << '\'';
		std::string es = ess.str();
		throw (NsException(es.c_str()));
	}
}

NsSkelJsonPtr NsSkelJsonParser::parseNull(istream &is, char &c) {
	//		cout << "parseNull" << endl;

	int u = is.get();
	if (u != 'u' && u != 'U') {
		throw (NsException("errorneous null value: u/U"));
	}
	int l1 = is.get();
	if (l1 != 'l' && l1 != 'L') {
		throw (NsException("errorneous null value: l/L 1"));
	}
	int l2 = is.get();
	if (l2 != 'l' && l2 != 'L') {
		throw (NsException("errorneous null value: l/L 2"));
	}

	is.get(c);

	return NsSkelJsonPtr(new NsSkelJsonNull());
}

NsSkelJsonPtr NsSkelJsonParser::parseObject(istream &is, char &c) {
	//		cout << "parseObject" << endl;

	shared_ptr<NsSkelJsonObject> object = make_shared<NsSkelJsonObject>();

	is.get(c);
	skipSpaces(is, c);
	if (c == '}') {
		is.get(c);
		return object;
	}
	while (true) {
		if (c != '"') {
			throw (NsException("Object key is not a JSON std::string: does not start with '\"'"));
		}
		NsSkelJsonStringPtr keyPtr = dynamic_pointer_cast<NsSkelJsonString>(parseString(is, c));

		skipSpaces(is, c);

		if (c != ':') {
			throw (NsException("Object badly formatted: no ':' between key and value"));
		}

		is.get(c);
		skipSpaces(is, c);

		NsSkelJsonPtr valuePtr = parseBasedOnFirst(is, c);

		std::pair<std::string, NsSkelJsonPtr> p(*keyPtr, valuePtr);
		object->insert(p);

		skipSpaces(is, c);
		if (c == '}') {
			break;
		}
		if (c != ',') {
			throw (NsException("Object badly formatted: no ',' between key-value pairs"));
		}

		is.get(c);
		skipSpaces(is, c);
	}

	is.get(c);
	return object;
}

NsSkelJsonPtr NsSkelJsonParser::parseArray(istream &is, char &c) {
	//		cout << "parseArray" << endl;

	shared_ptr<NsSkelJsonArray> array = make_shared<NsSkelJsonArray>();

	is.get(c);
	skipSpaces(is, c);
	if (c == ']') {
		is.get(c);
		return array;
	}
	while (true) {
		NsSkelJsonPtr toAdd = parseBasedOnFirst(is, c);

		array->push_back(toAdd);

		skipSpaces(is, c);

		if (c == ',') {
			is.get(c);
			skipSpaces(is, c);
		} else if (c == ']') {
			is.get(c);
			break;
		} else {
			std::stringstream ess;
			ess << "Unknown delimiter in array: '" << c << "'";
			std::string es = ess.str();
			throw (NsException(es.c_str()));
		}
	}

	return array;
}

NsSkelJsonPtr NsSkelJsonParser::parseString(istream &is, char &c) {
	//		cout << "parseString" << endl;

	std::stringstream ss;
	bool escaped = false;

	while (true) {
		if (is.eof()) {
			throw (NsException("Premature EOF when reading a std::string"));
		}
		is.get(c);
		if (c == '"' && !escaped) {
			break;
		}

		if (escaped) {
			if (c == 'n') {
				ss << endl;
			} else if (c == 't') {
				ss << '\t';
			} else {
				ss << c;
			}

			escaped = false;
		} else if (c == '\\') {
			escaped = true;
		} else {
			ss << c;
		}
	}

	std::string s = ss.str();
	is.get(c);
	return NsSkelJsonPtr(new NsSkelJsonString(s));
}

NsSkelJsonPtr NsSkelJsonParser::parseNumber(istream &is, char &c) {
	//		cout << "parseNumber" << endl;

	std::stringstream nss;
	nss << c;

	while ((c >= '0' && c <= '9') || c == '.') {
		is.get(c);
		if (is.eof()) {
			break;
		}

		nss << c;
	}

	double d;
	nss >> d;

	return NsSkelJsonPtr(new NsSkelJsonNumber(d));
}

NsSkelJsonPtr NsSkelJsonParser::parseBoolean(istream &is, char &c) {
	//		cout << "parseBoolean" << endl;

	if (c == 't' || c == 'T') { // Parsing true
		int r = is.get();
		if (r != 'r' && r != 'R') {
			throw (NsException("errorneous boolean TRUE value: r/R"));
		}
		int u = is.get();
		if (u != 'u' && u != 'U') {
			throw (NsException("errorneous boolean TRUE value: u/U"));
		}
		int e = is.get();
		if (e != 'e' && e != 'E') {
			throw (NsException("errorneous boolean TRUE value: e/E"));
		}

		is.get(c);

		return NsSkelJsonPtr(new NsSkelJsonBoolean(true));
	} else { // Parsing false
		int a = is.get();
		if (a != 'a' && a != 'A') {
			throw (NsException("errorneous boolean FALSE value: a/A"));
		}
		int l = is.get();
		if (l != 'l' && l != 'L') {
			throw (NsException("errorneous boolean FALSE value: l/L"));
		}
		int s = is.get();
		if (s != 's' && s != 'S') {
			throw (NsException("errorneous boolean FALSE value: s/S"));
		}
		int e = is.get();
		if (e != 'e' && e != 'E') {
			throw (NsException("errorneous boolean FALSE value: e/E"));
		}

		is.get(c);

		return NsSkelJsonPtr(new NsSkelJsonBoolean(false));
	}
}

namespace nanoservices {

// Checker function useful for casts

	void checkNsSkelJsonPtr(const NsSkelJsonPtr &p, const NsSkelJsonValueType &t) {
		if (!p) {
			throw NsException(NSE_POSITION, "Empty NsSkelJsonPtr provided");
		}

		if (p->type() != t) {
			std::stringstream ess;
			ess << "Requested target type is " << verboseNsSkelJsonType(t)
				<< ", but the supplied pointer is of type " << verboseNsSkelJsonType(p->type());
			throw (NsException(NSE_POSITION, ess));
		}
	}

/////////////////// Specializations for castNsSkelJsonPtr()

	template<class T>
	T _doCastNsSkelJsonPtr(const NsSkelJsonPtr &p, NsSkelJsonValueType &t) {
		checkNsSkelJsonPtr(p, t);

		return std::dynamic_pointer_cast<typename T::element_type>(p);
	}

	template<>
	NsSkelJsonNullPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NULL;
		return _doCastNsSkelJsonPtr<NsSkelJsonNullPtr>(p, t);
	}

	template<>
	NsSkelJsonObjectPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_OBJECT;
		return _doCastNsSkelJsonPtr<NsSkelJsonObjectPtr>(p, t);
	}

	template<>
	NsSkelJsonArrayPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_ARRAY;
		return _doCastNsSkelJsonPtr<NsSkelJsonArrayPtr>(p, t);
	}

	template<>
	NsSkelJsonStringPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_STRING;
		return _doCastNsSkelJsonPtr<NsSkelJsonStringPtr>(p, t);
	}

	template<>
	NsSkelJsonNumberPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doCastNsSkelJsonPtr<NsSkelJsonNumberPtr>(p, t);
	}

	template<>
	NsSkelJsonBooleanPtr castNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_BOOLEAN;
		return _doCastNsSkelJsonPtr<NsSkelJsonBooleanPtr>(p, t);
	}

/////////////////// Specializations for fromNsSkelJsonPtr()

	template<typename T>
	T _doFromNsSkelJsonPtr(const NsSkelJsonPtr &p, const NsSkelJsonValueType &t) {
		checkNsSkelJsonPtr(p, t);

		std::shared_ptr<T> op = castNsSkelJsonPtr<std::shared_ptr<T> >(p);
		return *op;
	}

	template<>
	NsSkelJsonObject fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_OBJECT;
		return _doFromNsSkelJsonPtr<NsSkelJsonObject>(p, t);
	}

	template<>
	std::map<std::string, NsSkelJsonPtr> fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_OBJECT;
		return _doFromNsSkelJsonPtr<NsSkelJsonObject>(p, t);
	}

	template<>
	NsSkelJsonArray fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_ARRAY;
		return _doFromNsSkelJsonPtr<NsSkelJsonArray>(p, t);
	}

	template<>
	std::vector<NsSkelJsonPtr> fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_ARRAY;
		return _doFromNsSkelJsonPtr<NsSkelJsonArray>(p, t);
	}

	template<>
	NsSkelJsonString fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_STRING;
		return _doFromNsSkelJsonPtr<NsSkelJsonString>(p, t);
	}

	template<>
	std::string fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_STRING;
		return _doFromNsSkelJsonPtr<NsSkelJsonString>(p, t);
	}

	template<>
	NsSkelJsonNumber fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t);
	}

	template<>
	double fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t).value();
	}

	template<>
	int64_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t).value();
	}

	template<>
	uint64_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t).value();
	}

	template<>
	int32_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t).value();
	}

	template<>
	uint32_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t).value();
	}

	template<>
	int16_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t).value();
	}

	template<>
	uint16_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t).value();
	}

	template<>
	int8_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t).value();
	}

	template<>
	uint8_t fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_NUMBER;
		return _doFromNsSkelJsonPtr<NsSkelJsonNumber>(p, t).value();
	}

	template<>
	NsSkelJsonBoolean fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_BOOLEAN;
		return _doFromNsSkelJsonPtr<NsSkelJsonBoolean>(p, t);
	}

	template<>
	bool fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = JSON_BOOLEAN;
		return _doFromNsSkelJsonPtr<NsSkelJsonBoolean>(p, t).value();
	}

	template<>
	NsSkelJsonNull fromNsSkelJsonPtr(const NsSkelJsonPtr &p) {
		NsSkelJsonValueType t = NsSkelJsonValueType::JSON_NULL;
		checkNsSkelJsonPtr(p, t);

		NsSkelJsonNull n;
		return n;
	}

/////////////////// Specializations for setNsSkelJsonPtr()

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonObject &v) {
		p = std::make_shared<NsSkelJsonObject>(v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, std::map<std::string, NsSkelJsonPtr> &v) {
		p = std::make_shared<NsSkelJsonObject>(v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonArray &v) {
		p = std::make_shared<NsSkelJsonArray>(v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, std::vector<NsSkelJsonPtr> &v) {
		p = std::make_shared<NsSkelJsonArray>(v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonString &v) {
		p = std::make_shared<NsSkelJsonString>(v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, std::string &v) {
		p = std::make_shared<NsSkelJsonString>(v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonNumber &v) {
		p = std::make_shared<NsSkelJsonNumber>(v.value());
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, double &v) {
		p = std::make_shared<NsSkelJsonNumber>(v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, int64_t &v) {
		p = std::make_shared<NsSkelJsonNumber>((double) v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, uint64_t &v) {
		p = std::make_shared<NsSkelJsonNumber>((double) v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, int32_t &v) {
		p = std::make_shared<NsSkelJsonNumber>((double) v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, uint32_t &v) {
		p = std::make_shared<NsSkelJsonNumber>((double) v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, int16_t &v) {
		p = std::make_shared<NsSkelJsonNumber>((double) v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, uint16_t &v) {
		p = std::make_shared<NsSkelJsonNumber>((double) v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, int8_t &v) {
		p = std::make_shared<NsSkelJsonNumber>((double) v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, uint8_t &v) {
		p = std::make_shared<NsSkelJsonNumber>((double) v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonBoolean &v) {
		p = std::make_shared<NsSkelJsonBoolean>(v.value());
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, bool &v) {
		p = std::make_shared<NsSkelJsonBoolean>(v);
	}

	template<>
	void setNsSkelJsonPtr(NsSkelJsonPtr &p, NsSkelJsonNull &v) {
		p = std::make_shared<NsSkelJsonNull>();
	}
} // namespace nanoservices
