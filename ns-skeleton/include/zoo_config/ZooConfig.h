#pragma once
#include <zookeeper.h>

#include "AbstractConfig.h"

namespace nanoservices {
	/**
	 * @see AbstractConfig
	 */
	class ZooConfig : public AbstractConfig {
	public:

		virtual void create(const std::string& path, nanoservices::NsSkelJsonPtr data);

		virtual nanoservices::NsSkelJsonPtr read(const std::string& path, bool desc);

		virtual void update(const std::string& path, nanoservices::NsSkelJsonPtr data);

		virtual void del(const std::string& path);
		virtual ~ZooConfig();
	private:
		ZooConfig(std::shared_ptr<ConfigValidator> validator);
		ZooConfig(ZooConfig&) = delete;
		ZooConfig& operator=(ZooConfig&) = delete;
		void init(const std::string& host, int port);
		void close();
		void createDict(const std::string& path, const std::string& parent_path, const std::string name, nanoservices::NsSkelJsonPtr data);
		void createSimpleNode(const std::string& path, const std::string& data="");
		void setNodeData(const std::string& path, const std::string& data);
		nanoservices::NsSkelJsonPtr readDict(const std::string& path, bool desc);
		void updateDict(const std::string& path, nanoservices::NsSkelJsonPtr data);
		std::vector<std::string> getChildren(const std::string& path);
		void delAll(const std::string& path);
		
		// PREFIX come from CMake
		std::string _prefix = PREFIX;
		std::vector<std::string> _prefixpath;
		void checkAndCreatePrefixPath();
		
		// Convert NsSkelJsonValueType into single byte and back
		std::vector<char> typedByte = {'\0', '\0', '\0', 's', 'd', 'b'};
		NsSkelJsonPtr jsonTypeFromData(const std::string& path, const std::string& data);
		
		zhandle_t* _zooconnection;
		std::shared_ptr<ConfigValidator> _validator;
		static const char delimiter = '/';

		friend class AbstractConfig;
	};
};
