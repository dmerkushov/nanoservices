#include "NsException.h"

class CVNoNodeData : public nanoservices::NsException {
public:
	CVNoNodeData(const std::string& what_arg);
	CVNoNodeData(const char* what_arg);
	virtual ~CVNoNodeData();
};
