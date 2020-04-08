#pragma once
#include <memory>

#include "ConfigValidator.h"
#include "NsSkelJson.h"

namespace nanoservices {
	class AbstractConfig {
	public:
		/**
		 * Get instance of current realisation of AbstractConfig
		 */
		static std::shared_ptr<AbstractConfig> instance();
		/**
		 * Initialisation connection to config source
		 * @param uri -- URI to config source
		 * @param validator -- validator for data used in create and update methods
		 */
		static void init(const std::string& uri, std::shared_ptr<ConfigValidator> validator = std::make_shared<ConfigValidator>()) throw(nanoservices::NsException);
		/**
		 * Close connection to config source
		 */
		static void close();
		/**
		 * Create config node at path with data.
		 *   If data define subnodes, they created too
		 * @param path -- path to creating node, all parent node must exists
		 * @param data
		 */
		virtual void create(const std::string& path, nanoservices::NsSkelJsonPtr data) = 0;
		/**
		 * Read config node data from path
		 * @param path -- path to node
		 */
		virtual nanoservices::NsSkelJsonPtr read(const std::string& path, bool desc = false) = 0;
		/**
		 * Update config node at path use data
		 * @param path -- path to updating node
		 * @param data
		 */
		virtual void update(const std::string& path, nanoservices::NsSkelJsonPtr data) = 0;
		/**
		 * Delete config node at path
		 *   All subnodes delete too.
		 * @param path -- path to node
		 */
		virtual void del(const std::string& path) = 0;

		virtual ~AbstractConfig(){};
	private:
		static std::shared_ptr<AbstractConfig> _instance;
		AbstractConfig(AbstractConfig&) = delete;
		AbstractConfig& operator=(AbstractConfig&) = delete;
	protected:
		AbstractConfig(){};
	};
};
