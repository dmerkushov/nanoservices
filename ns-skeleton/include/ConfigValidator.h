#pragma once

#include "NsSkelJson.h"

class ConfigValidator {
public:
	/**
	 * 
	 */
	virtual void validate(const std::string& path, nanoservices::NsSkelJsonPtr data);
	
	static const int32_t childrenCount = 1024;
	static const std::string dataFieldName;
	static const std::string descFieldName;
	static const int64_t maxDataSize = 1024L << 10;
	
private:
	virtual void validateObject(const std::string& path, const std::string& name, nanoservices::NsSkelJsonPtr data);
};
