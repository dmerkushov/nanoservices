#include <iostream>

#include "AbstractConfig.h"
#include "NsCmdLineParameters.h"

using namespace std;
using namespace nanoservices;

int main(int argc, char** argv) {
	map<char, NsCmdLineParameters::opt> prms = {
		{'p' , {"path", required_argument, 0, 'p'}},
		{'a' , {"data", required_argument, 0, 'a'}},
		{'c' , {"create", no_argument, 0, 'c'}},
		{'r' , {"read", no_argument, 0, 'r'}},
		{'u' , {"update", no_argument, 0, 'u'}},
		{'d' , {"delete", no_argument, 0, 'd'}},
		{'\0', {0,0,0,0}}
	};
	NsCmdLineParameters::init(prms, argc, argv);
	AbstractConfig::init(URL);
	if(NsCmdLineParameters::instance()->isParam("create")) {
		string path = NsCmdLineParameters::instance()->paramValue("path");
		string json = NsCmdLineParameters::instance()->paramValue("data");

		NsSkelJsonPtr obj = NsSkelJsonParser().parse(json);
		AbstractConfig::instance()->create(path, obj);
	} else if(NsCmdLineParameters::instance()->isParam("read")) {
		string path = NsCmdLineParameters::instance()->paramValue("path");

		cout << AbstractConfig::instance()->read(path)->serialize() << endl;
	} else if(NsCmdLineParameters::instance()->isParam("delete")) {
		string path = NsCmdLineParameters::instance()->paramValue("path");

		AbstractConfig::instance()->del(path);
	}
	AbstractConfig::close();
	return 0;
}
