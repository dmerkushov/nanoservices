#include <iostream>
#include <numeric>

#include "AbstractConfig.h"
#include "NsCmdLineParameters.h"

using namespace std;
using namespace nanoservices;

int main(int argc, char** argv) {
	map<char, NsCmdLineParameters::opt> prms = {
		{'p' , {"path", 'p', "PATH", 0, "Path to config node", 2}},
		{'a' , {"data", 'a', "DATA", 0, "Data for create or update operation", 2}},
		{'c' , {"create", 'c', 0, 0, "create config node at PATH with DATA", 1}},
		{'r' , {"read", 'r', 0, 0, "read config node data from PATH", 1}},
		{'u' , {"update", 'u', 0, 0, "update config node at PATH as DATA", 1}},
		{'d' , {"delete", 'd', 0, 0, "delete config node at PATH", 1}},
		{'\0', {0,0,0,0}}
	};
	NsCmdLineParameters::init(prms, argc, argv);
	AbstractConfig::init(URI);
	if(NsCmdLineParameters::instance()->isParam("create")) {
		string path = NsCmdLineParameters::instance()->paramValue("path");
		vector<string> json_parts = NsCmdLineParameters::instance()->paramValues("data");
		
		string json = std::accumulate(std::next(json_parts.begin()), json_parts.end(),
                                    json_parts[0], // start with first element
                                    [](string a, string b) {
								         return std::move(a) + ' ' + std::move(b);
								     });

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
