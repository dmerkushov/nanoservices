#include "NsException.h"

class CVInvalidType : public nanoservices::NsException {
public:
	CVInvalidType(const std::string& what_arg);
	CVInvalidType(const char* what_arg);
	virtual ~CVInvalidType();
};
