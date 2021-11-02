#include "CVInvalidType.h"

CVInvalidType::CVInvalidType(const std::string& what) : nanoservices::NsException(what) {
}

CVInvalidType::CVInvalidType(const char* what):nanoservices::NsException(what) {
}

CVInvalidType::~CVInvalidType() {
}
