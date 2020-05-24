#include "NsException.h"

class CVTooLongData : public nanoservices::NsException {
public:
	CVTooLongData(const std::string& what_arg);
	CVTooLongData(const char* what_arg);
	virtual ~CVTooLongData();
};
