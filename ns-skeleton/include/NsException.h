/*
 * Copyright 2019 dmerkushov.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * File:   NsException.h
 * Author: dmerkushov
 *
 * Created on February 21, 2019, 6:39 PM
 */

#ifndef NSEXCEPTION_H
#define NSEXCEPTION_H

#include <string>
#include <sstream>
#include <exception>
#include <vector>

#ifndef NSE_POSITION
#define NSE_POSITION (std::string("") + __FILE__ + ":" + std::to_string(__LINE__) + ": in " + __PRETTY_FUNCTION__)
#endif

#ifndef NSE_STACKTRACE_SIZE_MAX
#define NSE_STACKTRACE_SIZE_MAX (200)
#endif

// Need to use MsgPack directly here, since NsSerializer makes use of NsException
#define MSGPACK_USE_CPP03
#include <msgpack.hpp>

namespace msgpack2 = msgpack::v2;

namespace nanoservices {

class NsException : public std::exception {
public:
	NsException (std::string position, std::string shortDescription, const NsException &rootEx);
	NsException (std::string position, std::stringstream &shortDescription, const NsException &rootEx);
	NsException (std::string position, const NsException &rootEx);
	NsException (const std::string &position, const std::stringstream &shortDescription, const std::string &rootStacktrace, const std::string &rootShortDescription, const std::string &rootFullDescription);
	NsException (const NsException &rootEx);
	NsException (std::string position, std::string shortDescription);
	NsException (std::string position, std::stringstream &shortDescription);
	NsException (std::string shortDescription);
	NsException (std::stringstream &shortDescription);
	virtual ~NsException () _GLIBCXX_USE_NOEXCEPT;

	/**
	 * Inherited from std::exception
	 * @return The full description of the exception
	 */
	const char *what () const _GLIBCXX_USE_NOEXCEPT override;

	/**
	 *
	 * @return Stacktrace of the exception
	 */
	const std::string & stacktrace () const;

	/**
	 *
	 * @return Short description of the exception
	 */
	const std::string & shortDescription () const;

	/**
	 *
	 * @return Full description of the exception; the same string as would be constructed if calling what()
	 */
	const std::string & fullDescription () const;

	/**
	 *
	 * @return Full description of the root exception; empty string if no root exception provided
	 */
	const std::string & rootExceptionFullDescription () const;

private:
	std::string _shortDescription;
	std::string _fullDescription;
	std::string _stacktrace;
	std::string _rootExceptionFullDescription;

	/**
	 * Utility method to use in the constructors
	 * @param rootExStack
	 * @param rootExShort
	 * @param rootExFull
	 */
	void init (const std::string & rootExStack = "", const std::string & rootExShort = "", const std::string & rootExFull = "");

public:
	MSGPACK_DEFINE (_shortDescription, _fullDescription, _stacktrace, _rootExceptionFullDescription);
};

}

#endif /* NSEXCEPTION_H */

