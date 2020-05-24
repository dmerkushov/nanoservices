#include <unistd.h>
#include <sstream>
#include <cstring>

#include "NsUtils.h"

using namespace std;

namespace nanoservices {

	std::vector <std::string> string_split(std::string str, char ch) {
		stringstream ss(str);
		string item;
		std::vector <std::string> result;

		while (getline(ss, item, ch)) {
			result.push_back(move(item));
		}

		return result;
	}

	char* new_c_str(std::string str) {
		char* res = new char[str.size() + 1];
		strcpy(res, str.c_str());
		return res;
	}
};
