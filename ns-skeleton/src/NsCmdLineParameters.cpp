
/*
 * Copyright 2019 apetelin.
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
 * File:   NsCmdLineParameters.cpp
 * Author: apetelin
 *
 * Created on Dec 9, 2019, 1:03 PM
 */
 
#include <unistd.h>
#include <regex>
#include <iostream>

#include "NsCmdLineParameters.h"

using namespace std;
using namespace nanoservices;

std::shared_ptr<NsCmdLineParameters> NsCmdLineParameters::_instance;

void NsCmdLineParameters::init(std::map<char, NsCmdLineParameters::opt>& param_defs, int argc, char** argv) {
	_instance = std::shared_ptr<NsCmdLineParameters>(new NsCmdLineParameters(param_defs, argc, argv));
}

NsCmdLineParameters::NsCmdLineParameters(std::map<char, NsCmdLineParameters::opt>& param_defs, int argc, char** argv):_argc(argc), _argv(argv) {
	parse(param_defs);
}

namespace nanoservices {
	/**
	* Non standart split by delimeter for std::string
	* @param str -- string for splitting
	* @param ch -- delimeter character
	*/
	std::vector <std::string> string_split(std::string str, char ch) {
		stringstream ss(str);
		string item;
		std::vector <std::string> result;

		while (getline(ss, item, ch)) {
			result.push_back(move(item));
		}

		return result;
	}
	
	std::map<char, struct option> getOptionDefinitions(NsSkelJsonPtr paramsKeys) {
		std::map<char, struct option> long_opt;

		if(paramsKeys) {
			auto params = castNsSkelJsonPtr<NsSkelJsonArrayPtr>(paramsKeys);
			for (auto it = params->begin(); it != params->end(); ++it) {
				auto param = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(*it);
				if (param->count("key") == 0) {
					throw NsException(NSE_POSITION, "Parameter options not contains 'key'!");
				}
				if (fromNsSkelJsonPtr<string>(param->at("key")) == string("n")) {
					throw NsException(NSE_POSITION, "Parameter option 'key' = 'n' is reserved!");
				}
				if (fromNsSkelJsonPtr<string>(param->at("key")) == string("p")) {
					throw NsException(NSE_POSITION, "Parameter option 'key' = 'p' is reserved!");
				}
				if (param->count("long-key") == 0) {
					throw NsException(NSE_POSITION, "Parameter options not contains 'long-key'!");
				}
				if (fromNsSkelJsonPtr<string>(param->at("long-key")) == string("name")) {
					throw NsException(NSE_POSITION, "Parameter option 'long-key' = 'name' is reserved!");
				}
				if (fromNsSkelJsonPtr<string>(param->at("long-key")) == string("port")) {
					throw NsException(NSE_POSITION, "Parameter option 'long-key' = 'port' is reserved!");
				}
				if (param->count("isRequired") == 0) {
					throw NsException(NSE_POSITION, "Parameter options not contains 'isRequired'!");
				}

				string key = fromNsSkelJsonPtr<string>(param->at("key"));

				if (key.size() > 1) {
					throw NsException(NSE_POSITION, "Parameter option 'key' have size more 1 character!!");
				}

				string longKey_s = fromNsSkelJsonPtr<string>(param->at("long-key"));
				bool argsReq = fromNsSkelJsonPtr<bool>(param->at("isRequired"));
				char shortKey = key[0];
				char *longKey = new char[longKey_s.size() + 1];
				strcpy(longKey, longKey_s.c_str());

				long_opt[shortKey] = {longKey, argsReq ? required_argument : no_argument, 0, shortKey};
			}
		}

		// Option for change monitoring service name
		long_opt['n'] = {"name", required_argument, 0, 'n'};
		// Option for change service port
		long_opt['p'] = {"port", required_argument, 0, 'p'};
		// Sequence of option struct must be zero termninated
		long_opt['\0'] = {0, 0, 0, 0};

		return long_opt;
	}

};

char* NsCmdLineParameters::getShortOptions(std::map<char, NsCmdLineParameters::opt>& map) {
	std::vector<char> short_v;
	for(auto it = map.rbegin(); it != map.rend(); ++it) {
		short_v.push_back(it->first);
		switch(it->second.has_arg) {
			case required_argument:
				short_v.push_back(':');
				break;
			case no_argument:
				break;
			case optional_argument:
				short_v.push_back(':');
				short_v.push_back(':');
				break;
		}
	}

	char* res = new char[short_v.size()];
	copy(short_v.begin(), short_v.end(), res);
	return res;
}

NsCmdLineParameters::opt* NsCmdLineParameters::getLongOptions(std::map<char, NsCmdLineParameters::opt>& map) {
	opt* res = new opt[map.size()];
	int i = 0;
	for(auto it = map.rbegin(); it != map.rend(); ++it) {
		res[i++] = it->second;
	}

	return res;
}

void NsCmdLineParameters::parse(std::map<char, NsCmdLineParameters::opt>& map) {
	int opt;
	int option_index = 0;

	char* optionString = getShortOptions(map);
	auto options = getLongOptions(map);

	opterr = 0;

	while ((opt = getopt_long (_argc, _argv, optionString, options, &option_index)) != -1) {
		if(! (opt == '?' || opt == ':')) {
			_params[map[opt].name] = string_split(string((map[opt].has_arg == required_argument)?optarg:""), ' ');
		} else {
			_unparsed.push_back(_argv[optind-1]);
		}
	}

	for(int i = optind; i < _argc; i++) {
		_unparsed.push_back(_argv[i]);
	}
}

std::shared_ptr<NsCmdLineParameters> NsCmdLineParameters::instance () throw (NsException){
	if(!_instance) {
		throw NsException (NSE_POSITION, "Call of NsCmdLineParameters::instance() before calling NsSkeleton::init()");
	}
	return _instance;
}

bool NsCmdLineParameters::isParam(std::string keyName){
	return _params.count(keyName) > 0;
}

std::string NsCmdLineParameters::paramValue(std::string keyName) {
	auto paramsv = paramValues(keyName);
	if(paramsv.size() == 0) {
		return "";
	} 
	return paramsv[0];
}

std::vector <std::string> NsCmdLineParameters::paramValues(std::string keyName) {
	return _params.at(keyName);
}

std::vector<std::string> NsCmdLineParameters::unparsedParameters() {
	return _unparsed;
}
