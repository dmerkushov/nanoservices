#include "CVNoNodeData.h"

CVNoNodeData::CVNoNodeData(const std::string& what):nanoservices::NsException(what) {
}

CVNoNodeData::CVNoNodeData(const char* what):nanoservices::NsException(what) {
}

CVNoNodeData::~CVNoNodeData() {
}
