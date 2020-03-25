#include <iostream>

#include "ConfigValidator.h"
#include "CVInvalidType.h"
#include "CVTooManyChildren.h"
#include "CVTooLongData.h"
#include "CVNoNodeData.h"

using namespace std;
using namespace nanoservices;

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
		if(obj.size() > 1024) {
			throw CVTooManyChildren(string() + "Too many children at \"" + path + "\".");
		}
		if(!(obj.find("__description__") != obj.end())) {
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
		if(!(name == "__data__" || name == "__description__")) {
			throw CVNoNodeData(string() + "No data for \"" + path + "\".");
		}
		if(str.size() > 1024*1024) {
			throw CVTooLongData(string() + "Too long data as \"" + path + "\".");
		}
		break;
	}
	case JSON_NULL:
		break;
	default:
		throw CVInvalidType("Support only string and objects!");
		break;
	}
}


