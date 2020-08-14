
#include <thread>
#include <chrono>

#include "httplib.h"

using namespace httplib;

int main(int, char**) {
	Server serv;
	bool shutdown = false;
	serv.Get("/stop", [&](const Request& req, Response& res) {
		shutdown = true;
	});

	std::thread([&]() {serv.listen("0.0.0.0", 8080);}).detach();

	while(!shutdown) {
		::usleep(1);
	}

	serv.stop();

	return 0;
}
