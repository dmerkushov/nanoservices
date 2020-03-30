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

pair<NsSkelJsonPtr, string> FileConfig::goThroughPath(const std::string& path, nanoservices::NsSkelJsonPtr nodeobj) {
	auto vnodes = string_split(path.substr(1), '/');
	string last_node = vnodes.back();
	vnodes.pop_back();
	for(auto node : vnodes) {
		auto obj = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(nodeobj);
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
	return {nodeobj, last_node};
}

void FileConfig::create(const std::string& path, nanoservices::NsSkelJsonPtr data) {
	_validator->validate(path, data);
	NsSkelJsonPtr conf = readWhole("/");
	if(path[0] != '/') {
		throw NsException(NSE_POSITION, "Path must start from \'/\'!");
	}
	auto nodepair = goThroughPath(path, conf);
	auto addnode = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(nodepair.first);
	if(addnode->find(nodepair.second) != addnode->end()) {
		throw NsException(NSE_POSITION, "Path already exists");
	}
	(*addnode)[nodepair.second] = data;
	string content = conf->serialize();
	ofstream dataf(_configPath);
	dataf.write(content.c_str(), content.size());
}

NsSkelJsonPtr FileConfig::processData(NsSkelJsonPtr data, bool desc) {
	NsSkelJsonPtr processed;
	if(data->type() == JSON_OBJECT) {
		auto map = make_shared<NsSkelJsonObject>();
		NsSkelJsonPtr nodedata;	
		for(auto it: fromNsSkelJsonPtr<NsSkelJsonObject>(data)) {
			if(desc || it.first != ConfigValidator::descFieldName) {
				if(it.first != ConfigValidator::dataFieldName) {
					map->insert({it.first, processData(it.second, desc)});
				} else {
					nodedata = processData(it.second, desc);
				}
			}
		}
		if(map->size() > 0) {
			if(nodedata) {
				map->insert({ConfigValidator::dataFieldName, nodedata});
			}
			processed = map;
		} else {
			processed = nodedata;
		}
	} else {
		processed = data ;
	}
	return processed;
}

NsSkelJsonPtr FileConfig::read(const std::string& path, bool desc) {
	return processData(readWhole(path), desc);
}

NsSkelJsonPtr FileConfig::readWhole(const std::string& path) {
	if(path[0] != '/') {
		throw NsException(NSE_POSITION, "Path must start from \'/\'!");
	}
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
	NsSkelJsonPtr conf = readWhole("/");
	if(path[0] != '/') {
		throw NsException(NSE_POSITION, "Path must start from \'/\'!");
	}
	auto nodepair = goThroughPath(path, conf);
	auto addnode = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(nodepair.first);
	if(addnode->find(nodepair.second) == addnode->end()) {
		throw NsException(NSE_POSITION, "Path not exists");
	}
	(*addnode)[nodepair.second] = data;
	string content = conf->serialize();
	ofstream dataf(_configPath);
	dataf.write(content.c_str(), content.size());	
}

void FileConfig::del(const std::string& path) {
	NsSkelJsonPtr conf = readWhole("/");
	if(path[0] != '/') {
		throw NsException(NSE_POSITION, "Path must start from \'/\'!");
	}
	auto nodepair = goThroughPath(path, conf);
	auto delnode = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(nodepair.first);
	auto delit = delnode->find(nodepair.second);
	if(delit != delnode->end()) {
		delnode->erase(delit);
	}
	string content = conf->serialize();
	ofstream dataf(_configPath);
	dataf.write(content.c_str(), content.size());
}

FileConfig::~FileConfig() {
}

FileConfig::FileConfig(shared_ptr<ConfigValidator> validator):_validator(validator) {
}
