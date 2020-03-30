#include <cxxtest/TestSuite.h>

#include "AbstractConfig.h"
#include "TestActionCheck.h"

#include "CVInvalidType.h"
#include "CVNoNodeData.h"
#include "CVTooLongData.h"
#include "CVTooManyChildren.h"

using namespace std;
using namespace nanoservices;

class ZooConfigInitTest : public CxxTest::TestSuite {
public:
	void testInitMailformedURI(void) {
		TS_TRACE("Try init to mailformed URI");
		TS_TRACE("\tURI=" MAILFURI);
		TS_ASSERT_THROWS(AbstractConfig::init(MAILFURI), CONFIGEXCEPTION);
	}
	
	void testInitBadURI(void) {
		TS_TRACE("Try init to outer URI");
		TS_TRACE("\tURI=" BADURI);
		TS_ASSERT_THROWS(AbstractConfig::init(BADURI), CONFIGEXCEPTION);
	}

	void testInitURINoConfigSource(void) {
		TS_TRACE("Try init to bad URI (enabled, but without config source)");
		TS_TRACE("\tURI=" LESSBADURI);
		TS_ASSERT_THROWS_ANYTHING(AbstractConfig::init(LESSBADURI));
		TS_ASSERT_THROWS(AbstractConfig::init(LESSBADURI), CONFIGEXCEPTION);
	}

	void testInit(void) {
		TS_TRACE("Try init correct URI");
		TS_ASSERT_THROWS_NOTHING(AbstractConfig::init(URI));
		TS_TRACE("Second try init after successfull init");
		TS_ASSERT_THROWS(AbstractConfig::init(URI), CONFIGEXCEPTION);
	}

	void tearDown() {
		TS_TRACE("Set object to close (uninited) state");
		AbstractConfig::close();
	}
};

class ZooConfigInstanceTest : public CxxTest::TestSuite {
public:
	void testInstanceEmpty(void) {
		TS_TRACE("Try get instance without init");
		TS_ASSERT_THROWS(AbstractConfig::instance(), CONFIGEXCEPTION);
	}
};

class ZooConfigInstanceTest2 : public CxxTest::TestSuite {
public:
	void setUp() {
		TS_TRACE("Init object instance");
		AbstractConfig::init(URI);
	}

	void testInstanceEqual(void) {
		TS_TRACE("Get object instance pointer");
		auto a = AbstractConfig::instance();
		TS_TRACE("Check object instance pointer not null");
		TS_ASSERT_DIFFERS(a, nullptr);
		TS_TRACE("Second get object instance pointer");
		auto b = AbstractConfig::instance();
		TS_TRACE("Check equals for two object instance pointers");
		TS_ASSERT_EQUALS(a, b);
	}

	void tearDown() {
		TS_TRACE("Set object to close (uninited) state");
		AbstractConfig::close();
	}
};

class ZooConfigCreateBadTest : public CxxTest::TestSuite {
	char* bigbuff;
public:
	void setUp() {
		bigbuff = new char[1024*1024*2];
		AbstractConfig::init(URI);
	}

	void testCreateNull(void) {
		TS_TRACE("");
		TS_ASSERT_THROWS_ANYTHING(AbstractConfig::instance()->create("/test", nullptr));
	}

	void testCreateIncorrect(void) {
		TS_TRACE("");
		string str;
		NsSkelJsonPtr data = NsSkelJsonPtr (new NsSkelJsonString(str));
		TS_ASSERT_THROWS_ANYTHING(AbstractConfig::instance()->create("/test", data));
		TS_ASSERT_THROWS(AbstractConfig::instance()->create("/test", data), NsException);
	}

	void testCreateTooMany(void) {
		TS_TRACE("");
		map<std::string, NsSkelJsonPtr> map_;
		for(int i = 0; i < 2000; i++) {
			map_["child" + to_string(i)] = nullptr;
		}
		NsSkelJsonPtr obj1 = NsSkelJsonPtr (new NsSkelJsonObject(map_));
		map<std::string, NsSkelJsonPtr> map1;
		map1["data"] = obj1;
		NsSkelJsonPtr obj = NsSkelJsonPtr (new NsSkelJsonObject(map1));
		TS_ASSERT_THROWS_ANYTHING(AbstractConfig::instance()->create("/test", obj));
		TS_ASSERT_THROWS(AbstractConfig::instance()->create("/test", obj), CVTooManyChildren);
	}

	void testCreateBig(void) {
		TS_TRACE("");
		string str(bigbuff, 1024*1024*2);
		NsSkelJsonPtr data = NsSkelJsonPtr (new NsSkelJsonString(str));
		map<std::string, NsSkelJsonPtr> map;
		map["__data__"] = data;
		NsSkelJsonPtr obj = NsSkelJsonPtr (new NsSkelJsonObject(map));
		TS_ASSERT_THROWS_ANYTHING(AbstractConfig::instance()->create("/test", obj));
		TS_ASSERT_THROWS(AbstractConfig::instance()->create("/test", obj), CVTooLongData);
	}

	void testCreateNoData(void) {
		TS_TRACE("");
		map<std::string, NsSkelJsonPtr> map;
		map["data"] = NsSkelJsonPtr (new NsSkelJsonString("1234"));
		NsSkelJsonPtr obj = NsSkelJsonPtr (new NsSkelJsonObject(map));
		TS_ASSERT_THROWS_ANYTHING(AbstractConfig::instance()->create("/test", obj));
		TS_ASSERT_THROWS(AbstractConfig::instance()->create("/test", obj), CVNoNodeData);
	}

	void testCreateInvalidData(void) {
		TS_TRACE("");
		map<std::string, NsSkelJsonPtr> map;
		std::vector<NsSkelJsonPtr> v = {NsSkelJsonPtr (new NsSkelJsonNumber(1234))};
		map["__data__"] = NsSkelJsonPtr (new NsSkelJsonArray(v));
		NsSkelJsonPtr obj = NsSkelJsonPtr (new NsSkelJsonObject(map));
		TS_ASSERT_THROWS_ANYTHING(AbstractConfig::instance()->create("/test", obj));
		TS_ASSERT_THROWS(AbstractConfig::instance()->create("/test", obj), CVInvalidType);
	}

	void tearDown() {
		delete [] bigbuff;
		AbstractConfig::close();
	}
};

class ZooConfigCreateGoodTest : public CxxTest::TestSuite {
public:
	void setUp() {
		AbstractConfig::init(URI);
	}

	void testSimpleCreate(void) {
		TS_TRACE("");
		string str("bigbuff, 1024*1024*2");
		string json("{\"__data__\":\"bigbuff, 1024*1024*2\", \"host\":{\"__data__\":\"10.1.6.42\", \"__description__\":\"Хост сервиса\"}}");
		NsSkelJsonPtr obj = NsSkelJsonParser().parse(json);
		TS_ASSERT_THROWS_NOTHING(AbstractConfig::instance()->create("/test", obj));
		TAC_ASSERT_EQUALS("/test", str);
	}

	void tearDown() {
		AbstractConfig::close();
		TAC_CLEAN();
	}
};

class AbstractConfigCreateNotGoodTest : public CxxTest::TestSuite {
	string str[3] = {"test", "child1", "child2"};
public:
	void setUp() {
		TAC_INIT(str);
		AbstractConfig::init(URI);
	}
public:
	void testCreateCreated(void) {
		TS_TRACE("");
		map<std::string, NsSkelJsonPtr> map;
		map["__data__"] = NsSkelJsonPtr (new NsSkelJsonString("1234"));
		NsSkelJsonPtr obj = NsSkelJsonPtr (new NsSkelJsonObject(map));
		TS_ASSERT_THROWS_ANYTHING(AbstractConfig::instance()->create("/test", obj));
		TS_ASSERT_THROWS(AbstractConfig::instance()->create("/test", obj), CONFIGEXCEPTION);
	}

	void tearDown() {
		AbstractConfig::close();
		TAC_CLEAN();
	}
};

class ZooConfigReadTest : public CxxTest::TestSuite {
	string str[3] = {"test", "child1", "child2"};
public:
	void setUp() {
		TAC_INIT(str);
		AbstractConfig::init(URI);
	}

	void testReadOK(void) {
		TS_TRACE("");
		TS_ASSERT_THROWS_NOTHING(AbstractConfig::instance()->read("/test"));
		auto a = AbstractConfig::instance()->read("/test");
		auto obj = fromNsSkelJsonPtr<NsSkelJsonObject>(a);
		TS_ASSERT_EQUALS(fromNsSkelJsonPtr<string>(obj["__data__"]), str[0]);
		TS_ASSERT(obj.find(str[1]) != obj.end());
		TS_ASSERT_EQUALS(fromNsSkelJsonPtr<string>(obj[str[1]]), str[1]);
		TS_ASSERT(obj.find(str[2]) != obj.end());
		TS_ASSERT_EQUALS(fromNsSkelJsonPtr<string>(obj[str[2]]), str[2]);
	}

	void tearDown() {
		AbstractConfig::close();
		TAC_CLEAN();
	}

};

class ZooConfigUpdateTest : public CxxTest::TestSuite {
	string str[3] = {"test", "child1", "child2"};
	string newdata[3] = {"data1", "вася", "Петя"};
public:
	void setUp() {
		TAC_INIT(str);
		AbstractConfig::init(URI);
	}

	void testUpdateOK(void) {
		TS_TRACE("");
		NsSkelJsonPtr data = NsSkelJsonPtr (new NsSkelJsonString(newdata[0]));
		map<std::string, NsSkelJsonPtr> map;
		map["__data__"] = data;
		NsSkelJsonPtr obj = NsSkelJsonPtr (new NsSkelJsonObject(map));
		TS_ASSERT_THROWS_NOTHING(AbstractConfig::instance()->update("/test", obj));
		TAC_ASSERT_EQUALS("/test", newdata[0]);
	}

	void tearDown() {
		AbstractConfig::close();
		TAC_CLEAN();
	}

};

class ZooConfigDeleteTest : public CxxTest::TestSuite {
	string str[3] = {"test", "child1", "child2"};
public:
	void setUp() {
		TAC_INIT(str);
		AbstractConfig::init(URI);
	}

	void testDeleteOK(void) {
		TS_TRACE("");
		TS_ASSERT_THROWS_NOTHING(AbstractConfig::instance()->del(string() + "/" + str[0]));
		TAC_ASSERT_NOT_EXISTS("/test");

	}

	void tearDown() {
		AbstractConfig::close();
		TAC_CLEAN();
	}
};
