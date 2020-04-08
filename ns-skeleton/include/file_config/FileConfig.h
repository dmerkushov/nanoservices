#pragma once

#include "AbstractConfig.h"

namespace nanoservices {
	/**
	 * @see AbstractConfig
	 */
	class FileConfig : public AbstractConfig {
	public:
		virtual void create(const std::string& path, nanoservices::NsSkelJsonPtr data);
		
		virtual nanoservices::NsSkelJsonPtr read(const std::string& path, bool desc = false);
		
		virtual void update(const std::string& path, nanoservices::NsSkelJsonPtr data);
		
		virtual void del(const std::string& path);
		virtual ~FileConfig();
	private:
		FileConfig(std::shared_ptr<ConfigValidator> validator);
		FileConfig(FileConfig&) = delete;
		FileConfig& operator=(FileConfig&) = delete;
		void init(const std::string& configPath);

		// Utilite method for extract json subnode by subnode's path
		std::pair<nanoservices::NsSkelJsonPtr, std::string> goThroughPath(const std::string& path, nanoservices::NsSkelJsonPtr nodeobj);
		nanoservices::NsSkelJsonPtr readWhole(const std::string& path);
		nanoservices::NsSkelJsonPtr processData(nanoservices::NsSkelJsonPtr data, bool desc);

		std::shared_ptr<ConfigValidator> _validator;
		std::string _configPath;

		friend class AbstractConfig;
	};
};
