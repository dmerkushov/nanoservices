#include "NsSkeleton.h"
#include "NsCmdLineParameters.h"
#include "exchange/NsVoidArgs.h"
#include "exchange/NsVoidResult.h"

#include <iostream>

using namespace std;
using namespace nanoservices;

int main(int argc, char** argv) {
	map<char, NsCmdLineParameters::opt> prms = {
		{'n' , {"name", 'n', "NAME", 0, "Name of service to shutdown"}},
		{'p' , {"port", 'p', "PORT", 0, "Port of service to shutdown"}},
		{'h' , {"host", 'h', "HOST", 0, "Host of service to shutdown"}}
	};

	NsCmdLineParameters::init(prms, argc, argv);

	if(NsCmdLineParameters::instance()->isParam("name")) {
	
		auto serviceName = make_shared<string>(NsCmdLineParameters::instance()->paramValue("name"));
		NsSkeleton::init(*serviceName);

		auto service = NsSkelRpcRegistry::instance()->getService(serviceName);
		
		char *host = (char*)service->host()->c_str();
		if(NsCmdLineParameters::instance()->isParam("host")) {
			string host_s = NsCmdLineParameters::instance()->paramValue("host");
			host = new char[host_s.size()+1];
			strcpy(host, host_s.c_str());
		}
		uint16_t port = service->port();
		if(NsCmdLineParameters::instance()->isParam("port")) {
			port = stoi(NsCmdLineParameters::instance()->paramValue("port"));
		}
		string method = "shutdown";
		NsSerialized args;
		std::shared_ptr<NsRpcRequest> request = make_shared<NsRpcRequest>(method, args, false);
		
		sendPackRpcRequest(serviceName, host, port, request);
		return 0;
	}
	cerr << "Parameter 'name' must be set." << endl;
	return 1;
}
