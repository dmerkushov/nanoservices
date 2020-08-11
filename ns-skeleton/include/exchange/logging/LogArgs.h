#ifndef LOGARGS_H
#define LOGARGS_H

#include <string>

#include "../../NsSerializer.h"

enum LogLevel {
	Trace = 100,
	Debug = 200,
	Info = 300,
	Warning = 400,
	Error = 500,
	Fatal = 600
};

NSSERIALIZER_ADD_ENUM (LogLevel);

class LogArgs {
public:
	/**
	 * Logging level for the record
	 */
	LogLevel logLevel;

	/**
	 * The service that had sent the logging record
	 */
	std::string sourceService;

	/**
	 * Text of the log record
	 */
	std::string text;

	/**
	 * Time that this has happened
	 */
	std::string time;

	NSSERIALIZER_PREPARE (logLevel, sourceService, text, time);
};

#endif // LOGARGS_H
