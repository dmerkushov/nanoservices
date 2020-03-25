#include <string.h>
#include <chrono>
#include <thread>
#include <future>
#include <regex>

#include "ZooConfig.h"

using namespace std;
using namespace nanoservices;

shared_ptr<AbstractConfig> AbstractConfig::_instance = nullptr;

void AbstractConfig::init(const string& uri, shared_ptr<ConfigValidator> validator) throw(NsException) {
	if(_instance) {
		throw NsException(NSE_POSITION, "Already connected!");
	}
	regex pieces_regex("zoo://([a-z0-9\\.]+):([0-9]+)");
	smatch pieces_match;
 
	string host = "";
	string port_s = "";
	int port = 0;
    
	if (regex_match(uri, pieces_match, pieces_regex)) {
		// Extract data from 1st parentsizes
		host = pieces_match[1].str();
		// Extract data from 2nd parentsizes
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

void ZooConfig::init(const string& host, int port) throw(NsException) {
	class ZHCloser {
	public:
		ZHCloser(zhandle_t* zh):_zh(zh) {};
		~ZHCloser(){if(_zh){zookeeper_close(_zh);}};
		zhandle_t* _zh;
	};
	string url = host;
	url += ":" + to_string(port);
	auto log_fp_ = fopen("/dev/null", "w");
	zoo_set_log_stream(log_fp_);
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
	promise<int> connect_wait;
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
	auto f = connect_wait.get_future();
	thread([&](){
		this_thread::sleep_for(std::chrono::milliseconds(timeoutms));
		connect_wait.set_exception(make_exception_ptr(NsException(NSE_POSITION, "Coudn't connect!")));
	}).detach();
	int state = f.get();
	if(state == ZOO_CONNECTED_STATE) {
		this->_zooconnection = zh;
		c._zh = 0;
		// Clear promise from context
		zoo_set_context(zh, 0);
		// Clear watcher from context
		zoo_set_watcher(zh, 0);

	} else {
		throw NsException(NSE_POSITION, "Coudn't connect!");
	}
}

void ZooConfig::create(const std::string& path, nanoservices::NsSkelJsonPtr data) {
	_validator->validate(path, data);
	int res = zoo_exists(_zooconnection, _prefixpath[2].c_str(), 0, 0);
	if(res == ZNONODE) {
		for(auto p: _prefixpath) {
			int res = zoo_exists(_zooconnection, p.c_str(), 0, 0);
			if(res == ZNONODE) {
				res = zoo_create(_zooconnection, p.c_str(), 0, 0, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0);
			}
		}
	}
	createDict(_prefixpath[2] + path, "", "", data);
}

void ZooConfig::createDict(const std::string& path, const std::string& parent_path, const std::string name, nanoservices::NsSkelJsonPtr data) {
	switch(data->type()) {
	case JSON_OBJECT: {
		auto obj = fromNsSkelJsonPtr<NsSkelJsonObject>(data);
		createSimpleNode(path);
		for(auto it: obj) {
			createDict(path + "/" + it.first, path, it.first, it.second);
		}
		break;
	}
	case JSON_STRING: {
		auto str = fromNsSkelJsonPtr<string>(data);
		if(name == "__data__") {
			setNodeData(parent_path, str);
		}
		if(name == "__description__") {
			createSimpleNode(path, str);
		}
		break;
	}
	case JSON_NULL: {
		createSimpleNode(path);
		break;
	}
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

NsSkelJsonPtr ZooConfig::read(const string& path) {
	return readDict(_prefixpath[2] + path);
}

vector<string> ZooConfig::getChildren(const string& path) {
	promise<vector<string>> chp;
	auto f1 = chp.get_future();
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
	}, &chp);
	if(res != ZOK) {
		throw NsException(NSE_POSITION, zerror(res));
	}
	auto children = f1.get();
	return children;
}

NsSkelJsonPtr ZooConfig::readDict(const string& path) {
	auto children = getChildren(path);
	promise<string> datap;
	auto f2 = datap.get_future();
	int res = zoo_aget(_zooconnection, path.c_str(), 0, [](int rc, const char *value, int value_len, const struct Stat *stat, const void *data){
		auto pptr = (promise<string>*)data;
		if(rc != ZOK) {
			pptr->set_exception(make_exception_ptr(NsException(NSE_POSITION, zerror(rc))));
			return;
		}
		pptr->set_value(move(string(value, value_len)));
	}, &datap);
	if(res != ZOK) {
		throw NsException(NSE_POSITION, zerror(res));
	}
	auto data = f2.get();
	NsSkelJsonPtr result = NsSkelJsonPtr (new NsSkelJsonNull());
	if(children.size() > 0) {
		map<std::string, NsSkelJsonPtr> map;
		if(data.size() > 0) {
			map["__data__"] = NsSkelJsonPtr (new NsSkelJsonString(data));
		}
		for(auto child: children) {
			map[child] = readDict(path+"/"+child);
		}
		result = NsSkelJsonPtr (new NsSkelJsonObject(map));
	} else {
		if(data.size() > 0) {
			result = NsSkelJsonPtr (new NsSkelJsonString(data));
		}
	}
	return result;
}

void ZooConfig::update(const std::string& path, nanoservices::NsSkelJsonPtr data) {
	_validator->validate(path, data);
	updateDict(_prefixpath[2] + path, data);
}

void ZooConfig::updateDict(const std::string& path, nanoservices::NsSkelJsonPtr data) {
	switch(data->type()) {
	case JSON_OBJECT: {
		auto obj = fromNsSkelJsonPtr<NsSkelJsonObject>(data);
		for(auto it: obj) {
			if(it.first != "__data__") {
				updateDict(path + "/" + it.first, it.second);
			} else {
				auto str = fromNsSkelJsonPtr<string>(it.second);
				setNodeData(path, str);
			}
		}
		break;
	}
	case JSON_STRING: {
		auto str = fromNsSkelJsonPtr<string>(data);
		setNodeData(path, str);
		break;
	}
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
	delAll(_prefixpath[2] + path);
}

void ZooConfig::delAll(const std::string& path) {
	auto children = getChildren(path);
	for(auto child: children) {
		delAll(path + "/" + child);
	}
	int res = zoo_delete(_zooconnection, path.c_str(), -1);
	if(res != ZOK) {
		throw NsException(NSE_POSITION, zerror(res));
	}
}

ZooConfig::ZooConfig(std::shared_ptr<ConfigValidator> validator):_zooconnection(0), _validator(validator) {
}

ZooConfig::~ZooConfig() {
	if(_zooconnection){
		zookeeper_close(_zooconnection);
	}
}
