#pragma once
#include <zookeeper.h>

#include "AbstractConfig.h"

namespace nanoservices {
	class ZooConfig : public AbstractConfig {
	public:
		/**
		*
		*/
		virtual void create(const std::string& path, nanoservices::NsSkelJsonPtr data);
		/**
		*
		*/
		virtual nanoservices::NsSkelJsonPtr read(const std::string& path);
		/**
		*
		*/
		virtual void update(const std::string& path, nanoservices::NsSkelJsonPtr data);
		/**
		*
		*/
		virtual void del(const std::string& path);
		virtual ~ZooConfig();
	private:
		ZooConfig(std::shared_ptr<ConfigValidator> validator);
		ZooConfig(ZooConfig&) = delete;
		ZooConfig& operator=(ZooConfig&) = delete;
		void init(const std::string& host, int port) throw(nanoservices::NsException);
		void close();
		void createDict(const std::string& path, const std::string& parent_path, const std::string name, nanoservices::NsSkelJsonPtr data);
		void createSimpleNode(const std::string& path, const std::string& data="");
		void setNodeData(const std::string& path, const std::string& data);
		nanoservices::NsSkelJsonPtr readDict(const std::string& path);
		void updateDict(const std::string& path, nanoservices::NsSkelJsonPtr data);
		std::vector<std::string> getChildren(const std::string& path);
		void delAll(const std::string& path);

		std::string _prefixpath[3] = {"/ru", "/ru/cniiag", "/ru/cniiag/webmaps"};
		zhandle_t* _zooconnection;
		std::shared_ptr<ConfigValidator> _validator;

		friend class AbstractConfig;
	};
};
