#pragma once

#include "NsSkelJson.h"

class ConfigValidator {
public:
	/**
	 * Valide data, path used only for generate errors messages
	 *   Now check:
	 *    - count of subnodes at one node
	 *    - size of node data 
	 *    - using appropriate types for define node data
	 *    - definition of data for leaf subnodes
	 * @param path
	 * @param data
	 */
	virtual void validate(const std::string& path, nanoservices::NsSkelJsonPtr data);
	
	static const int32_t MAX_CHILDREN_COUNT = 1024;
	static const std::string dataFieldName;
	static const std::string descFieldName;
	static const int64_t MAX_DATA_SIZE = 1024L << 10;
	
private:
	virtual void validateObject(const std::string& path, const std::string& name, nanoservices::NsSkelJsonPtr data);
};
