#include <iostream>

#include "ConfigValidator.h"
#include "CVInvalidType.h"
#include "CVTooManyChildren.h"
#include "CVTooLongData.h"
#include "CVNoNodeData.h"

using namespace std;
using namespace nanoservices;

const string ConfigValidator::dataFieldName = "__data__";
const string ConfigValidator::descFieldName = "__description__";


// TODO: Move field count and field names to public static constants
void ConfigValidator::validate(const string& path, nanoservices::NsSkelJsonPtr data) {
	if(!data) {
		throw NsException("data is null");
	}
	if(data->type() != JSON_OBJECT) {
		throw CVInvalidType("Data must be JSON object!");
	}
	validateObject(path, "", data);
}

void ConfigValidator::validateObject(const std::string& path, const std::string& name, nanoservices::NsSkelJsonPtr data) {
	switch(data->type()) {
	case JSON_OBJECT: {
		auto obj = fromNsSkelJsonPtr<NsSkelJsonObject>(data);
		if(obj.size() > childrenCount) {
			throw CVTooManyChildren(string() + "Too many children at \"" + path + "\".");
		}
		if(!(obj.find(descFieldName) != obj.end())) {
			// TODO: use logger(?!)
			cerr << "[WARNING] Node " << path << " hasn't description!" << endl;
		}
		for(auto it: obj) {
			validateObject(path + "/" + it.first, it.first, it.second);
		}
		break;
	}
	case JSON_STRING: {
		auto str = fromNsSkelJsonPtr<string>(data);
		if(str.size() > maxDataSize) {
			throw CVTooLongData(string() + "Too long data as \"" + path + "\".");
		}
		// Descriptions string only
		if(name == descFieldName) {
			break;
		}
	}
	case JSON_NUMBER:
	case JSON_BOOLEAN:
	case JSON_NULL:
		if(name != dataFieldName) {
			throw CVNoNodeData(string() + "No data for \"" + path + "\".");
		}
		break;
	default:
		throw CVInvalidType("Json type " + verboseNsSkelJsonType(data->type()) + " not supported!");
		break;
	}
}


