#pragma once

#include "AbstractConfig.h"

namespace nanoservices {
	class FileConfig : public AbstractConfig {
	public:
		/**
		*
		*/
		virtual void create(const std::string& path, nanoservices::NsSkelJsonPtr data);
		/**
		*
		*/
		virtual nanoservices::NsSkelJsonPtr read(const std::string& path, bool desc = false);
		/**
		*
		*/
		virtual void update(const std::string& path, nanoservices::NsSkelJsonPtr data);
		/**
		*
		*/
		virtual void del(const std::string& path);
		virtual ~FileConfig();
	private:
		FileConfig(std::shared_ptr<ConfigValidator> validator);
		FileConfig(FileConfig&) = delete;
		FileConfig& operator=(FileConfig&) = delete;
		void init(const std::string& configPath);

		std::shared_ptr<ConfigValidator> _validator;
		std::string _configPath;

		friend class AbstractConfig;
	};
};
