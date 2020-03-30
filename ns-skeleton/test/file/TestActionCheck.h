#pragma once
#include <fstream>

#define PATH "/etc/nanoservices/global.conf.json"

#define MAILFURI PATH
#define BADURI "file:///etc/google"
#define LESSBADURI "file:///var/lib/apt/lists/lock"
#define URI "file://" PATH

#define CONFIGEXCEPTION NsException
#define TAC_ASSERT_EQUALS(path, str) { \
	ifstream idataf (PATH); \
	NsSkelJsonParser parser; \
	NsSkelJsonObjectPtr dataj = parser.typedParse<NsSkelJsonObjectPtr> (idataf); \
	string ipath = string(path).substr(1); \
	auto node = (*dataj)[ipath]; \
	TS_ASSERT_EQUALS(node->type(), JSON_OBJECT); \
	node = fromNsSkelJsonPtr<NsSkelJsonObject>(node)[ConfigValidator::dataFieldName]; \
	TS_ASSERT_EQUALS(fromNsSkelJsonPtr<NsSkelJsonString>(node), str); \
}

#define TAC_CLEAN() { \
	ifstream idataf (PATH); \
	NsSkelJsonParser parser; \
	NsSkelJsonObjectPtr dataj = parser.typedParse<NsSkelJsonObjectPtr> (idataf); \
	if(dataj->find("test") != dataj->end()) {\
		dataj->erase(dataj->find("test")); \
	} \
	string content = dataj->serialize(); \
	ofstream odataf(PATH); \
	odataf.write(content.c_str(), content.size()); \
}

#define TAC_ASSERT_NOT_EXISTS(path) { \
	ifstream idataf (PATH); \
	NsSkelJsonParser parser; \
	NsSkelJsonObjectPtr dataj = parser.typedParse<NsSkelJsonObjectPtr> (idataf); \
	string ipath = string(path).substr(1); \
	TS_ASSERT(dataj->find(ipath) == dataj->end()); \
}

#define TAC_INIT(str) { \
	ifstream idataf (PATH); \
	NsSkelJsonParser parser; \
	NsSkelJsonObjectPtr dataj = parser.typedParse<NsSkelJsonObjectPtr> (idataf); \
	string json = "{\"__data__\":\"" + str[0] + "\", \"" + str[1] + "\":{\"__data__\":\"" + str[1] + "\"}, \"" + str[2] + "\":{\"__data__\":\"" + str[2] + "\"}}"; \
	(*dataj)[str[0]] = NsSkelJsonParser().parse(json); \
	string content = dataj->serialize(); \
	ofstream odataf(PATH); \
	odataf.write(content.c_str(), content.size()); \
}

namespace test_file {
	
};
