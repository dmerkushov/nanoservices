#include <string.h>
#include <chrono>
#include <thread>
#include <future>
#include <regex>

#include <iostream>

#include "ZooConfig.h"
#include "NsUtils.h"

using namespace std;
using namespace nanoservices;

shared_ptr<AbstractConfig> AbstractConfig::_instance = nullptr;

void AbstractConfig::init(const string& uri, shared_ptr<ConfigValidator> validator) {
	if(_instance) {
		throw NsException(NSE_POSITION, "Already connected!");
	}
	regex pieces_regex("zoo://([a-z0-9\\.]+):([0-9]+)");
	smatch pieces_match;

	string host = "";
	string port_s = "";
	int port = 0;

	if (regex_match(uri, pieces_match, pieces_regex)) {
		// Extract data from 1st parentheses
		host = pieces_match[1].str();
		// Extract data from 2nd parentheses
		port_s = pieces_match[2].str();
		port = stoi(port_s);
	} else {
		throw NsException(NSE_POSITION, "Incorrect URI");
	}
	shared_ptr<ZooConfig> inst = shared_ptr<ZooConfig>(new ZooConfig(validator));
	inst->init(host, port);
	_instance = inst;
}

void AbstractConfig::close() {
	_instance = nullptr;
};

shared_ptr<AbstractConfig> AbstractConfig::instance() {
	if(!_instance) {
		throw NsException(NSE_POSITION, "Not connected!");
	}
	return _instance;
}

void ZooConfig::init(const string& host, int port) {
	class ZHCloser {
	public:
		ZHCloser(zhandle_t* zh):_zh(zh) {};
		~ZHCloser(){if(_zh){zookeeper_close(_zh);}};
		zhandle_t* _zh;
	};
	string url = host;
	url += ":" + to_string(port);
	// TODO: Use file path from ENV 
	auto log_fp_ = fopen("/dev/null", "w");
	zoo_set_log_stream(log_fp_);
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
	promise<int> connect_wait;
	// TODO: move to constant
	int timeoutms = 3000;
	zhandle_t* zh = zookeeper_init(url.c_str(), [](zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
		if(watcherCtx) {
			promise<int>* pp = (promise<int>*)watcherCtx;
			if(state != ZOO_CONNECTING_STATE) {
				pp->set_value(state);
			}
		}
		return;
	}
	, timeoutms, 0, &connect_wait, 0);
	if(!zh) {
		throw NsException(NSE_POSITION, strerror(errno));
	}
	ZHCloser c(zh);
	int state = -1;
	auto future_conn = connect_wait.get_future();
	thread([&](){
		this_thread::sleep_for(std::chrono::milliseconds(timeoutms));
		if(state == -1) {
			connect_wait.set_exception(make_exception_ptr(NsException(NSE_POSITION, "Couldn't connect to zookeeper!")));
		}
	}).detach();
	state = future_conn.get();
	if(state == ZOO_CONNECTED_STATE) {
		this->_zooconnection = zh;
		c._zh = 0;
		// Clear promise from context
		zoo_set_context(zh, 0);
		// Clear watcher from context
		zoo_set_watcher(zh, 0);

	} else {
		throw NsException(NSE_POSITION, "Coudn't connect to zookeeper!");
	}
}

void ZooConfig::create(const std::string& path, nanoservices::NsSkelJsonPtr data) {
	_validator->validate(path, data);
	checkAndCreatePrefixPath();
	createDict(_prefix + path, "", "", data);
}

void ZooConfig::checkAndCreatePrefixPath() {
	string path = "";
	for(auto p: _prefixpath) {
		path += delimiter + p;
		int res = zoo_exists(_zooconnection, path.c_str(), 0, 0);
		if(res == ZNONODE) {
			res = zoo_create(_zooconnection, path.c_str(), 0, 0, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0);
		}
	}
}

void ZooConfig::createDict(const std::string& path, const std::string& parent_path, const std::string name, nanoservices::NsSkelJsonPtr data) {
	switch(data->type()) {
	case JSON_OBJECT: {
		auto obj = fromNsSkelJsonPtr<NsSkelJsonObject>(data);
		createSimpleNode(path);
		for(auto it: obj) {
			createDict(path + delimiter + it.first, path, it.first, it.second);
		}
		break;
	}
	case JSON_STRING: 
	case JSON_NUMBER:
	case JSON_BOOLEAN: {
		auto str = data->serialize();
		if(name == ConfigValidator::dataFieldName) {
			setNodeData(parent_path, typedByte[data->type()] + str);
		}
		if(name == ConfigValidator::descFieldName) {
			createSimpleNode(path, typedByte[JSON_STRING] + str);
		}
		break;
	}
	case JSON_NULL: {
		createSimpleNode(path);
		break;
	}
	case JSON_ARRAY:
		throw NsException(NSE_POSITION, "Json type " + verboseNsSkelJsonType(data->type()) + " not supported!");
	default:
		break;
	}
}

void ZooConfig::createSimpleNode(const string& path, const string& data) {
	int res = zoo_create(_zooconnection, path.c_str(), data.c_str(), data.size(), &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0);
	if(res != ZOK) {
		throw NsException(NSE_POSITION, zerror(res));
	}
}

NsSkelJsonPtr ZooConfig::read(const string& path, bool desc) {
	return readDict(_prefix + path, desc);
}

vector<string> ZooConfig::getChildren(const string& path) {
	promise<vector<string>> child_v_p;
	auto child_v_f = child_v_p.get_future();
	int res = zoo_aget_children(_zooconnection, path.c_str(), 0, [](int rc, const struct String_vector *strings, const void *data){
		auto pptr = (promise<vector<string>>*)data;
		if(rc != ZOK) {
			pptr->set_exception(make_exception_ptr(NsException(NSE_POSITION, zerror(rc))));
			return;
		}
		vector<string> ch;
		if(strings) {
			for(int i=0; i<strings->count; i++) {
				ch.push_back(move(string(strings->data[i])));
			}
		}
		pptr->set_value(move(ch));
	}, &child_v_p);
	if(res != ZOK) {
		throw NsException(NSE_POSITION, zerror(res));
	}
	return child_v_f.get();
}

NsSkelJsonPtr ZooConfig::jsonTypeFromData(const string& path, const string& data) {
	char type = data[0];
	switch(type) {
	case 's':
	case 'd':
	case 'b':
		break;
	default:
		stringstream ess;
		ess <<  "Data at \"" << path << "\" is corrupt or uses an unknown type specifier!";
		throw NsException(NSE_POSITION, ess);
	}
	string to_parse = data.substr(1);
	NsSkelJsonParser parser;
	return parser.parse(to_parse);
}

NsSkelJsonPtr ZooConfig::readDict(const string& path, bool desc) {
	auto children = getChildren(path);
	promise<string> data_p;
	auto data_f = data_p.get_future();
	int res = zoo_aget(_zooconnection, path.c_str(), 0, [](int rc, const char *value, int value_len, const struct Stat *stat, const void *data){
		auto pptr = (promise<string>*)data;
		if(rc != ZOK) {
			pptr->set_exception(make_exception_ptr(NsException(NSE_POSITION, zerror(rc))));
			return;
		}
		pptr->set_value(move(string(value, value_len)));
	}, &data_p);
	if(res != ZOK) {
		throw NsException(NSE_POSITION, zerror(res));
	}
	auto data = data_f.get();
	NsSkelJsonPtr result = NsSkelJsonPtr (new NsSkelJsonNull());
	bool descPresent = find(children.begin(), children.end(), ConfigValidator::descFieldName) != children.end();
	if((children.size() - ((descPresent && !desc)?1:0)) > 0) {
		map<std::string, NsSkelJsonPtr> map;
		if(data.size() > 0) {
			map[ConfigValidator::dataFieldName] = jsonTypeFromData(path, data);
		}
		for(auto child: children) {
			if(child != ConfigValidator::descFieldName || desc) {
				map[child] = readDict(path+delimiter+child, desc);
			}
		}
		result = NsSkelJsonPtr (new NsSkelJsonObject(map));
	} else {
		if(data.size() > 0) {
			result = jsonTypeFromData(path, data);
		}
	}
	return result;
}

void ZooConfig::update(const std::string& path, nanoservices::NsSkelJsonPtr data) {
	_validator->validate(path, data);
	updateDict(_prefix + path, data);
}

void ZooConfig::updateDict(const std::string& path, nanoservices::NsSkelJsonPtr data) {
	switch(data->type()) {
	case JSON_OBJECT: {
		auto obj = fromNsSkelJsonPtr<NsSkelJsonObject>(data);
		for(auto it: obj) {
			if(it.first != ConfigValidator::dataFieldName) {
				updateDict(path + delimiter + it.first, it.second);
			} else {
				auto str = it.second->serialize();
				setNodeData(path, typedByte[it.second->type()] + str);
			}
		}
		break;
	}
	case JSON_NULL:
	case JSON_STRING: 
	case JSON_NUMBER:
	case JSON_BOOLEAN: {
		auto str = data->serialize();
		setNodeData(path, typedByte[data->type()] + str);
		break;
	}
	case JSON_ARRAY:
		throw NsException(NSE_POSITION, "Json type " + verboseNsSkelJsonType(data->type()) + " not supported!");
	default:
		break;
	}
}

void ZooConfig::setNodeData(const std::string& path, const std::string& data) {
	int res = zoo_set(_zooconnection, path.c_str(), data.c_str(), data.size(), -1);
	if(res != ZOK) {
		throw NsException(NSE_POSITION, zerror(res));
	}
}

void ZooConfig::del(const std::string& path) {
	delAll(_prefix + path);
}

void ZooConfig::delAll(const std::string& path) {
	auto children = getChildren(path);
	for(auto child: children) {
		delAll(path + delimiter + child);
	}
	int res = zoo_delete(_zooconnection, path.c_str(), -1);
	if(res != ZOK) {
		throw NsException(NSE_POSITION, zerror(res));
	}
}

ZooConfig::ZooConfig(std::shared_ptr<ConfigValidator> validator):_zooconnection(0), _validator(validator) {
	_prefixpath = string_split(_prefix.substr(1), delimiter);
}

ZooConfig::~ZooConfig() {
	if(_zooconnection){
		zookeeper_close(_zooconnection);
	}
}
