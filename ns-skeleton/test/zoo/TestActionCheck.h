#pragma once
#include <future>
#include <vector>
#include <zookeeper.h>

#define MAILFURI "zoo.google.com"
#define BADURI "zoo://google:80"
#define LESSBADURI "zoo://127.0.0.1:2180"
#define URI "zoo://127.0.0.1:2181"

#define CONFIGEXCEPTION NsException
#define TAC_ASSERT_EQUALS(path, str) { \
	zhandle_t* zh = zookeeper_init("127.0.0.1:2181", 0, 5000, 0, 0, 0); \
	TS_ASSERT_EQUALS(zoo_exists(zh, "/ru/cniiag/webmaps" path, 0, 0), ZOK); \
	promise<string> ps; \
	auto f1 = ps.get_future(); \
	TS_ASSERT_EQUALS(zoo_aget(zh, "/ru/cniiag/webmaps" path, 0, [](int rc, const char *value, int value_len, const struct Stat *stat, const void *data){ \
		auto p = (promise<string>*)data; \
		p->set_value(move(string(value, value_len))); \
	}, &ps), ZOK);\
	auto zoostr = f1.get();\
	zookeeper_close(zh);\
	TS_ASSERT_EQUALS("s\"" + str + "\"", zoostr); \
}

#define TAC_CLEAN() { \
	zhandle_t* zh = zookeeper_init("127.0.0.1:2181", 0, 500, 0, 0, 0); \
	test_zoo::removeNode(zh, "/ru"); \
	zookeeper_close(zh); \
}

#define TAC_ASSERT_NOT_EXISTS(path) { \
	zhandle_t* zh = zookeeper_init("127.0.0.1:2181", 0, 5000, 0, 0, 0); \
	TS_ASSERT_EQUALS(zoo_exists(zh, "/ru/cniiag/webmaps" path, 0, 0), ZNONODE); \
	zookeeper_close(zh); \
}

#define TAC_INIT(str) { \
	std::string base = "/ru/cniiag/webmaps"; \
	zhandle_t* zh = zookeeper_init("127.0.0.1:2181", 0, 500, 0, 0, 0); \
	zoo_create(zh, "/ru", 0, 0, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0); \
	zoo_create(zh, "/ru/cniiag", 0, 0, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0); \
	zoo_create(zh, "/ru/cniiag/webmaps", 0, 0, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0); \
	base += "/" + str[0]; \
	std::string data = "s\"" + str[0] + "\""; \
	zoo_create(zh, base.c_str(), data.c_str(), data.size(), &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0); \
	base += "/" + str[1]; \
	data = "s\"" + str[1] + "\""; \
	zoo_create(zh, base.c_str(), data.c_str(), data.size(), &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0); \
	base = "/ru/cniiag/webmaps/" + str[0] + "/" + str[2]; \
	data = "s\"" + str[2] + "\""; \
	zoo_create(zh, base.c_str(), data.c_str(), data.size(), &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0); \
	zookeeper_close(zh); \
}

namespace test_zoo {
	std::vector<std::string> getChildren(zhandle_t* zh, const std::string& path) {
		std::promise<std::vector<std::string>> chp;
		auto f1 = chp.get_future();
		TS_ASSERT_EQUALS(zoo_aget_children(zh, path.c_str(), 0, [](int rc, const struct String_vector *strings, const void *data){
			auto pptr = (std::promise<std::vector<std::string>>*)data;
			std::vector<std::string> ch;
			if(strings) {
				for(int i=0; i<strings->count; i++) {
					ch.push_back(std::move(std::string(strings->data[i])));
				}
			}
			pptr->set_value(std::move(ch));
		}, &chp), ZOK);
		return f1.get();
	}
	
	void removeNode(zhandle_t* zh, const std::string& path) {
		for(auto child: getChildren(zh, path)) {
			removeNode(zh, path+"/"+child);
		}
		TS_ASSERT_EQUALS(zoo_delete(zh, path.c_str(), -1), ZOK);
	}
};
