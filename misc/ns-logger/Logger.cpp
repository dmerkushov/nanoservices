#include <iostream>
#include <mutex>

#include "Logger.h"

using namespace std;
using namespace nanoservices;

static mutex _queueMutex;

ostream& operator <<(ostream& is, LogLevel& level) {
	switch(level) {
	case Trace:
		is << "TRACE";
		break;
	case Debug:
		is << "DEBUG";
		break;
	case Info:
		is << "INFO";
		break;
	case Warning:
		is << "WARN";
		break;
	case Error:
		is << "ERROR";
		break;
	case Fatal:
		is << "FATAL";
		break;
	}
	return is;
}

ostream& operator <<(ostream& is, shared_ptr<LogArgs> &args) {
	return is << args->time << " [" << args->sourceService << "] " << args->logLevel << " " << args->text;
}

shared_ptr<NsVoidResult> Logger::processRequest(shared_ptr<LogArgs> args) {
	unique_lock<mutex> lock(_queueMutex);
	_queue.push_back(args);
	return make_shared<NsVoidResult>();
};

void Logger::process() {
	if(_queue.empty()) return;
	unique_lock<mutex> lock(_queueMutex);
	auto p = _queue.front();
	cerr << p << endl;
	_queue.pop_front();
}
