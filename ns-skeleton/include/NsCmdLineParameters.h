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
 * File:   NsCmdLineParameters.h
 * Author: apetelin
 *
 * Created on Dec 9, 2019, 1:03 PM
 */

#ifndef NSCMDLINEPARAMETERS_H
#define NSCMDLINEPARAMETERS_H

#include <argp.h>
#include "NsSkelJson.h"

namespace nanoservices {

	class NsSkeleton;

	class NsCmdLineParameters final {
	public:
		/**
		 * Get the instance of NsCmdLineParameters for command line parameters of the current running nanoservice
		 * @return
		 */
		static std::shared_ptr<NsCmdLineParameters> instance();
		/**
		 * Check specific key in parsed command line parameters 
		 * @param keyName
		 */
		bool isParam(std::string keyName);

		/**
		 * Get values for specific key from parsed command line parameters
		 * @param keyName
		 */
		std::vector<std::string> paramValues(std::string keyName);

		/**
		 * Get first value for specific key from parsed command line parameters
		 * @param keyName
		 */
		std::string paramValue(std::string keyName);
		/**
		 * Get unparsed command line parameters
		 * @return
		 */
		std::vector<std::string> unparsedParameters();
		
		/**
		 * Alias for option type from argp.h
		 */
		using opt = struct argp_option;
		/**
		 * Initializing function
		 * @param param_defs -- command line parameters definitions
		 */
		static void init(std::map<char, opt>& param_defs, int argc, char** argv);
	private:
		NsCmdLineParameters(std::map<char, opt>& param_defs, int argc, char** argv);
		NsCmdLineParameters (const NsCmdLineParameters& orig) = delete;
		void operator= (const NsCmdLineParameters& orig) = delete;

		void parse(std::map<char, opt>& map);

		opt* getLongOptions(std::map<char, opt>& map);

		std::map<std::string, std::vector<std::string>> _params;
		std::vector<std::string> _unparsed;
		static std::shared_ptr<NsCmdLineParameters> _instance;
		int _argc;
		char** _argv;

		friend class NsSkeleton;
	};
	
	/**
	 * Convert json from config to map of argp option definitions
	 * @param paramsKeys -- json object, usually extracted from service config
	 */
	extern std::map<char, struct argp_option> getOptionDefinitions(NsSkelJsonPtr paramsKeys);

};

#endif//NSCMDLINEPARAMETERS_H
