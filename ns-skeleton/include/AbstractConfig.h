#pragma once
#include <memory>

#include "ConfigValidator.h"
#include "NsSkelJson.h"

namespace nanoservices {
	class AbstractConfig {
	public:
		/**
		 *
		 */
		static std::shared_ptr<AbstractConfig> instance();
		/**
		 *
		 */
		static void init(const std::string& uri, std::shared_ptr<ConfigValidator> validator = std::make_shared<ConfigValidator>()) throw(nanoservices::NsException);
		/**
		 *
		 */
		static void close();
		/**
		 *
		 */
		virtual void create(const std::string& path, nanoservices::NsSkelJsonPtr data) = 0;
		/**
		 *
		 */
		virtual nanoservices::NsSkelJsonPtr read(const std::string& path, bool desc = false) = 0;
		/**
		 *
		 */
		virtual void update(const std::string& path, nanoservices::NsSkelJsonPtr data) = 0;
		/**
		 *
		 */
		virtual void del(const std::string& path) = 0;
		/**
		 *
		 */
		virtual ~AbstractConfig(){};
	private:
		static std::shared_ptr<AbstractConfig> _instance;
		AbstractConfig(AbstractConfig&) = delete;
		AbstractConfig& operator=(AbstractConfig&) = delete;
	protected:
		AbstractConfig(){};
	};
};
