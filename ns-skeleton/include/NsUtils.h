#pragma once

#include <string>
#include <vector>

namespace nanoservices {
	
	/**
	 * Non standart split by delimeter for std::string
	 * @param str -- string for splitting
	 * @param ch -- delimeter character
	 */
	extern std::vector <std::string> string_split(std::string str, char ch);
	
	/**
	 * Non standart allocator for c_str() from std::string
	 * @param str
	 */
	extern char* new_c_str(std::string str);
};
