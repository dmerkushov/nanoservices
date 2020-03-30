#include <unistd.h>
#include <sstream>

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
};
