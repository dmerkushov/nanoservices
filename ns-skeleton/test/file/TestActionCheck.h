#pragma once
#include <fstream>

#define PATH "/etc/nanoservices/global.conf.json"

#define MAILFURI PATH
#define BADURI "file:///etc/google"
#define LESSBADURI "file:///var/lib/apt/lists/lock"
#define URI "file://" PATH

#define CONFIGEXCEPTION NsException
#define TAC_ASSERT_EQUALS(path, str) { \
}

#define TAC_CLEAN() { \
	ifstream idataf (PATH); \
	NsSkelJsonParser parser; \
	NsSkelJsonObjectPtr dataj = parser.typedParse<NsSkelJsonObjectPtr> (idataf); \
	dataj->erase(dataj->find("test")); \
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
	string json = "{\"__data__\":\"" + str[0] + "\", \"" + str[1] + "\":\"" + str[1] + "\", \"" + str[2] + "\":\"" + str[2] + "\"}"; \
	(*dataj)[str[0]] = NsSkelJsonParser().parse(json); \
	string content = dataj->serialize(); \
	ofstream odataf(PATH); \
	odataf.write(content.c_str(), content.size()); \
}
