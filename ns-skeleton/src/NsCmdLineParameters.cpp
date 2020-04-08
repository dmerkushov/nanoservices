
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
 
#include "NsCmdLineParameters.h"
#include "NsUtils.h"

using namespace std;
using namespace nanoservices;

std::shared_ptr<NsCmdLineParameters> NsCmdLineParameters::_instance;

namespace nanoservices {
	std::map<char, struct argp_option> getOptionDefinitions(NsSkelJsonPtr paramsKeys) {
		std::map<char, struct argp_option> argp_opt;

		if(paramsKeys) {
			auto params = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(paramsKeys);
			for (auto it = params->begin(); it != params->end(); ++it) {
				if (it->first == string("name")) {
					throw NsException(NSE_POSITION, "Parameter option 'name' is reserved!");
				}
				if (it->first == string("port")) {
					throw NsException(NSE_POSITION, "Parameter option 'port' is reserved!");
				}
				auto param = castNsSkelJsonPtr<NsSkelJsonObjectPtr>(it->second);
				if (param->count("key") == 0) {
					throw NsException(NSE_POSITION, "Options for parameter \""+it->first+"\" not contains 'key'!");
				}
				if (fromNsSkelJsonPtr<string>(param->at("key")) == string("n")) {
					throw NsException(NSE_POSITION, "Parameter option 'key' = 'n' is reserved!");
				}
				if (fromNsSkelJsonPtr<string>(param->at("key")) == string("p")) {
					throw NsException(NSE_POSITION, "Parameter option 'key' = 'p' is reserved!");
				}
				if (param->count("isRequired") == 0) {
					throw NsException(NSE_POSITION, "Options for parameter \""+it->first+"\" not contains 'isRequired'!");
				}
				if (param->count("desc") == 0) {
					throw NsException(NSE_POSITION, "Options for parameter \""+it->first+"\" not contains 'desc'!");
				}
				bool argsReq = fromNsSkelJsonPtr<bool>(param->at("isRequired"));
				if (argsReq && param->count("argName") == 0) {
					throw NsException(NSE_POSITION, "Options for parameter \""+it->first+"\" not contains 'argName'!");
				}

				string key = fromNsSkelJsonPtr<string>(param->at("key"));

				if (key.size() > 1) {
					throw NsException(NSE_POSITION, "Parameter option 'key' have size more 1 character!!");
				}

				string longKey = it->first;
				string argName;
				string desc = fromNsSkelJsonPtr<string>(param->at("desc"));
				if(argsReq) {
					argName = fromNsSkelJsonPtr<string>(param->at("argName"));
				}
				
				char shortKey = key[0];

				argp_opt[shortKey] = {new_c_str(longKey), shortKey, argsReq ? new_c_str(argName) : 0, 0, new_c_str(desc), 0};
			}
		}

		// Option for change monitoring service name
		argp_opt['n'] = {"name", 'n', "SERVICE", 0, "Change name to SERVICE", 0};
		// Option for change service port
		argp_opt['p'] = {"port", 'p', "PORT", 0, "Change binding port to PORT", 0};
		// Sequence of option struct must be zero termninated
		argp_opt['\0'] = {0};

		return argp_opt;
	}
};

void NsCmdLineParameters::init(std::map<char, NsCmdLineParameters::opt>& param_defs, int argc, char** argv) {
	// if no argument provided, no instance reinitialised
	if(argc || !_instance) {
		_instance = std::shared_ptr<NsCmdLineParameters>(new NsCmdLineParameters(param_defs, argc, argv));
	}
}

NsCmdLineParameters::NsCmdLineParameters(std::map<char, NsCmdLineParameters::opt>& param_defs, int argc, char** argv):_argc(argc), _argv(argv) {
	// if no argument provided, no parse doing
	if(argc) {
		parse(param_defs);
	}
}

NsCmdLineParameters::opt* NsCmdLineParameters::getLongOptions(std::map<char, NsCmdLineParameters::opt>& map) {
	opt* res = new opt[map.size()];
	int i = 0;
	for(auto it = map.rbegin(); it != map.rend(); ++it) {
		res[i++] = it->second;
	}

	return res;
}

void NsCmdLineParameters::parse(std::map<char, NsCmdLineParameters::opt>& opts) {
	int optind = _argc;
	auto options = getLongOptions(opts);
	struct argp argp = { options, [](int key, char *arg, struct argp_state *state) -> error_t {
		auto [that, opts] = *((pair<NsCmdLineParameters*, map<char, NsCmdLineParameters::opt>*>*)state->input);
		if(key == ARGP_KEY_ARG) {
			that->_unparsed.push_back(arg);
		} else if(opts->count(key) != 0) {
			that->_params.insert({(*opts)[key].name, string_split((arg ? arg : ""), ' ')});
		}
		return 0;
	}, 0, "\013Powered by nanoservices library v" VERSION };
	auto data = make_pair(this, &opts);
	argp_parse (&argp, _argc, _argv, 0, &optind, &data);
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
