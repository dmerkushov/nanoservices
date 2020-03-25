#include <sstream>
#include <iostream>
#include <fstream>
#include <regex>

#include "FileConfig.h"
#include "NsCmdLineParameters.h"

using namespace std;
using namespace nanoservices;

shared_ptr<AbstractConfig> AbstractConfig::_instance = nullptr;

void AbstractConfig::init(const string& uri, shared_ptr<ConfigValidator> validator) throw(NsException) {
	if(_instance) {
		throw NsException(NSE_POSITION, "Already connected!");
	}
	regex pieces_regex("file://(.+)");
	smatch pieces_match;
 
	string path = "";
    
	if (regex_match(uri, pieces_match, pieces_regex)) {
		// Extract data from 1st parentsizes
		path = pieces_match[1].str();
	} else {
		throw NsException("Incorrect URI");
	}
	shared_ptr<FileConfig> inst = shared_ptr<FileConfig>(new FileConfig(validator));
	inst->init(path);
	_instance = inst;
}

void AbstractConfig::close() {
	_instance = nullptr;
};

shared_ptr<AbstractConfig> AbstractConfig::instance() {
	if(!_instance) {
		throw NsException("Not connected!");
	}
	return _instance;
}

void FileConfig::init(const std::string& configPath) {
	ifstream conff(configPath);
	if(conff.fail()) {
		throw NsException(NSE_POSITION, "Config file cann't readen!");
	}
	_configPath = configPath;
}

void FileConfig::create(const std::string& path, nanoservices::NsSkelJsonPtr data) {
	_validator->validate(path, data);
	NsSkelJsonPtr conf = read("/"), nodeobj = conf;
	auto vnodes = string_split(path.substr(1), '/');
	string last_node = vnodes.back();
	vnodes.pop_back();
	for(auto node : vnodes) {
		auto obj = static_pointer_cast<NsSkelJsonObject, NsSkelJsonValueBase>(nodeobj);
		if(obj->find (node) != obj->end ()) {
			if((*obj)[node]->type() == JSON_OBJECT) {
				nodeobj = (*obj)[node];
			} else {
				throw NsException(NSE_POSITION, "Part of path not json-object");
			}
		} else {
			throw NsException(NSE_POSITION, "Part of path not found");
		}
	}
	auto addnode = static_pointer_cast<NsSkelJsonObject, NsSkelJsonValueBase>(nodeobj);
	if(addnode->find(last_node) != addnode->end()) {
		throw NsException(NSE_POSITION, "Path already exists");
	}
	(*addnode)[last_node] = data;
	string content = conf->serialize();
	ofstream dataf(_configPath);
	dataf.write(content.c_str(), content.size());
}

NsSkelJsonPtr FileConfig::read(const std::string& path) {
	ifstream dataf (_configPath);
	NsSkelJsonParser parser;
	NsSkelJsonObjectPtr dataj = parser.typedParse<NsSkelJsonObjectPtr> (dataf);
	NsSkelJsonPtr result = dataj;
	for(auto node : string_split(path.substr(1), '/')) {
		if(result->type() == JSON_OBJECT) {
			auto obj = fromNsSkelJsonPtr<NsSkelJsonObject>(result);
			if(obj.find (node) != obj.end ()) {
				result = obj[node];
			} else {
				throw NsException(NSE_POSITION, "Part of path not found");
			}
		} else {
			throw NsException(NSE_POSITION, "Part of path not json-object");
		}
	}
	return result;
}

void FileConfig::update(const std::string& path, nanoservices::NsSkelJsonPtr data) {
	_validator->validate(path, data);
	
}

void FileConfig::del(const std::string& path) {
	NsSkelJsonPtr conf = read("/"), nodeobj = conf;
	auto vnodes = string_split(path.substr(1), '/');
}

FileConfig::~FileConfig() {
}

FileConfig::FileConfig(shared_ptr<ConfigValidator> validator):_validator(validator) {
}
