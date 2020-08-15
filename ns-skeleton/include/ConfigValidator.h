#pragma once

#include "NsSkelJson.h"

class ConfigValidator {
public:
	/**
	 * Valide configuration data
	 *   By now, checks are:
	 * <ul>
	 * <li>count of subnodes at one node
	 * <li>size of node data 
	 * <li>usage of appropriate types to define node data
	 * <li>definition of data for leaf subnodes
	 * </ul>
	 * @param path used only to generate error messages
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
