#include "CVTooLongData.h"

CVTooLongData::CVTooLongData(const std::string& what):nanoservices::NsException(what) {
}

CVTooLongData::CVTooLongData(const char* what):nanoservices::NsException(what) {
}

CVTooLongData::~CVTooLongData() {
}
