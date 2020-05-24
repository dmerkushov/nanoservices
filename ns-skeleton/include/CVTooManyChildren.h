#include "NsException.h"

class CVTooManyChildren : public nanoservices::NsException {
public:
	CVTooManyChildren(const std::string& what_arg);
	CVTooManyChildren(const char* what_arg);
	virtual ~CVTooManyChildren();
};
