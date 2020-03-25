#pragma once

#include "NsSkelJson.h"

class ConfigValidator {
public:
	/**
	 * 
	 */
	virtual void validate(const std::string& path, nanoservices::NsSkelJsonPtr data);
private:
	virtual void validateObject(const std::string& path, const std::string& name, nanoservices::NsSkelJsonPtr data);
};
