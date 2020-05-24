#include "CVTooManyChildren.h"

CVTooManyChildren::CVTooManyChildren(const std::string& what):nanoservices::NsException(what) {
}

CVTooManyChildren::CVTooManyChildren(const char* what):nanoservices::NsException(what) {
}

CVTooManyChildren::~CVTooManyChildren() {
}
