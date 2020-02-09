//
// Created by dmerkushov on 2/9/20.
//

#include "NsCircuitBreaker.h"

#include <chrono>

#include "NsSkelConfiguration.h"

#define NS_CURRENT_TIMESTAMP (std::chrono::duration_cast<std::chrono::milliseconds>( \
            std::chrono::system_clock::now().time_since_epoch() \
            ).count())

using namespace std;
using namespace nanoservices;

CircuitBreakerStatistics::CircuitBreakerStatistics(uint64_t window) :
		_window(window) {
}

double CircuitBreakerStatistics::successfulShare() {
	uint64_t currentTimestamp = NS_CURRENT_TIMESTAMP;

	vector<CallResult>::iterator statsIt = _stats.begin();
	int successfulCount = 0;
	int totalCount;
	while (statsIt != _stats.end()) {
		if (statsIt->timestamp < currentTimestamp - _window) {
			statsIt = _stats.erase(statsIt);
			continue;
		}

		totalCount++;
		if (statsIt->success) {
			successfulCount++;
		}

		statsIt++;
	}

	if (totalCount == 0) {
		if (NsSkelConfiguration::instance()->getParameter<bool>(
				CONFIG_PARAMNAME_NSCIRCUITBREAKER_STATS_NODATAISGOOD
		)) {
			return 1.0;
		}
		return 0.0;
	}

	return 1.0 * successfulCount / totalCount;
}

void CircuitBreakerStatistics::registerCallResult(bool success) {
	CallResult callResult;
	callResult.success = success;
	callResult.timestamp = NS_CURRENT_TIMESTAMP;

	_stats.push_back(callResult);
}