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
 * File:   NsSkelConfiguration.h
 * Author: dmerkushov
 *
 * Created on February 28, 2019, 3:12 PM
 */

#ifndef NSSKELCONFIGURATION_H
#define NSSKELCONFIGURATION_H

#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "NsSkelJson.h"
#include "NsException.h"

extern "C" {
void nsSkelConfig(const std::string &serviceName, const std::string &configName) throw(nanoservices::NsException);
}

namespace nanoservices {

	class NsSkelConfiguration final {
	public:
		/**
		 * Though NsSkelConfiguration is considered singleton, any nanoservice can read any other's configuration by calling this constructor with the other's name. It is not recommended, yet.
		 * @param serviceName
		 */
		NsSkelConfiguration(const std::string &serviceName,
							const std::string &configName) throw(nanoservices::NsException);

		/**
		 * Get the instance of NsSkelConfiguration for the current running nanoservice
		 * @return
		 */
		static std::shared_ptr<NsSkelConfiguration> instance() throw(nanoservices::NsException);

		/**
		 * Get the current nanoservice's name
		 */
		std::shared_ptr<std::string> getServiceName() throw(NsException);

		/**
		 * Get the current nanoservice's config name
		 */
		std::shared_ptr<std::string> getConfigName() throw(NsException);

		/**
		 * Do we have the given parameter in the configuration?
		 * @param paramName
		 * @return
		 */
		bool hasParameter(std::string paramName);

		/**
		 * Get map with services configuration
		 */
		NsSkelJsonObjectPtr getServices();
		
		/**
		 * Template method: set a default value for a configuration parameter, so we can later use getParameter() calls without explicitly setting the default value every time.
		 *
		 * The method has effect for a given parameter only on the first call and if no parameter with such a name is provided in the configuration.
		 *
		 * The template will be instantiated explicitly for the following parameter types:
		 * <ul>
		 * <li>int64_t
		 * <li>double
		 * <li>std::string
		 * <li>bool
		 * </ul>
		 * @param paramName
		 * @param defaultVal
		 */
		template<typename T1>
		void setParameterDefault(std::string paramName, T1 defaultVal) throw(nanoservices::NsException) {
			if (!hasParameter(paramName)) {
				NsSkelJsonPtr v;

				setNsSkelJsonPtr<T1>(v, defaultVal);

				_configuration->insert(std::pair<std::string, NsSkelJsonPtr>(paramName, v));
			}
		}

	private:

		/**
		 * A special template struct to provide default parameter values to the templated getParameter() method
		 *
		 * The template will be instantiated explicitly for the following parameter types:
		 * <ul>
		 * <li>int64_t (default is 0)
		 * <li>double (default is 0.0)
		 * <li>std::string (default is "")
		 * <li>bool (default is true)
		 * </ul>
		 * @return
		 */
		template<typename T0>
		struct defaultArg {

			static T0 get() {
				return T0();
			}
		};

	public:

		/**
		 * Template method: get a configuration parameter.
		 *
		 * The template will be instantiated explicitly for the following parameter types:
		 * <ul>
		 * <li>int64_t (default is 0)
		 * <li>double (default is 0.0)
		 * <li>std::string (default is "")
		 * <li>bool (default is true)
		 * </ul>
		 * @param paramName
		 * @param defaultVal Used if the parameter is not provided in the configuration, and not set by setParameterDefault()
		 * @see defaultArg
		 */
		template<typename T2>
		T2 getParameter(const std::string paramName, T2 defaultVal = defaultArg<T2>::get()) throw(NsException) {
			try {
				if (!hasParameter(paramName)) {
					return defaultVal;
				}
				NsSkelJsonPtr v = _configuration->at(paramName);

				T2 returned = fromNsSkelJsonPtr<T2>(v);

				return returned;
			} catch (NsException &ex) {
				std::stringstream ess;
				ess << "NS Exception when reading parameter \"" << paramName << "\" : " << ex.what();
				throw NsException(NSE_POSITION, ess);
			} catch (std::exception &ex) {
				std::stringstream ess;
				ess << "Error when reading parameter \"" << paramName << "\" : " << ex.what();
				throw NsException(NSE_POSITION, ess);
			}
		}

	private:
		NsSkelConfiguration(const NsSkelConfiguration &orig) = delete;

		void operator=(const NsSkelConfiguration &orig) = delete;

		std::shared_ptr<std::string> _serviceName;
		std::shared_ptr<std::string> _configName;
		NsSkelJsonObjectPtr _configuration;
		NsSkelJsonObjectPtr _services;
	};

	template<>
	struct NsSkelConfiguration::defaultArg<int64_t> {

		static int64_t get() {
			return 0;
		}
	};

	template<>
	struct NsSkelConfiguration::defaultArg<double> {

		static double get() {
			return 0.0;
		}
	};

	template<>
	struct NsSkelConfiguration::defaultArg<std::string> {

		static std::string get() {
			return "";
		}
	};

	template<>
	struct NsSkelConfiguration::defaultArg<bool> {

		static bool get() {
			return true;
		}
	};

	template<>
	struct NsSkelConfiguration::defaultArg<NsSkelJsonPtr> {

		static NsSkelJsonPtr get() {
			NsSkelJsonPtr p;
			NsSkelJsonNull n;
			setNsSkelJsonPtr<NsSkelJsonNull>(p, n);
			return p;
		}
	};

	template void
	NsSkelConfiguration::setParameterDefault<int64_t>(std::string, int64_t) throw(nanoservices::NsException);

	template void
	NsSkelConfiguration::setParameterDefault<double>(std::string, double) throw(nanoservices::NsException);

	template void
	NsSkelConfiguration::setParameterDefault<std::string>(std::string, std::string) throw(nanoservices::NsException);

	template void NsSkelConfiguration::setParameterDefault<bool>(std::string, bool) throw(nanoservices::NsException);

	template int64_t NsSkelConfiguration::getParameter<int64_t>(std::string paramName,
																int64_t defaultVal) throw(nanoservices::NsException);

	template double NsSkelConfiguration::getParameter<double>(std::string paramName,
															  double defaultVal) throw(nanoservices::NsException);

	template std::string NsSkelConfiguration::getParameter<std::string>(std::string paramName,
																		std::string defaultVal) throw(nanoservices::NsException);

	template bool
	NsSkelConfiguration::getParameter<bool>(std::string paramName, bool defaultVal) throw(nanoservices::NsException);
}

#endif /* NSSKELCONFIGURATION_H */

