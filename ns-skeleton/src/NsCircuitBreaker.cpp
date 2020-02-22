//
// Created by dmerkushov on 2/9/20.
//

#include "NsCircuitBreaker.h"

#include <chrono>
#include <mutex>

#include "NsSkelConfiguration.h"

#define NS_CURRENT_TIMESTAMP (std::chrono::duration_cast<std::chrono::milliseconds>( \
            std::chrono::system_clock::now().time_since_epoch() \
            ).count())

using namespace std;
using namespace nanoservices;

mutex statsMutex;

/**
 * @brief Lock for the statistics data for exclusive use
 */
class StatsLock final {
public:
	StatsLock() {
		statsMutex.lock();
	}

	~StatsLock() {
		statsMutex.unlock();
	}
};

CircuitBreakerStatistics::CircuitBreakerStatistics(uint64_t temporalWindow) throw(NsException) :
		_temporalWindow(temporalWindow) {

	if (temporalWindow == 0) {
		stringstream ess;
		ess << "supplied temporalWindow is 0: " << temporalWindow;
		throw (NsException(NSE_POSITION, ess));
	}
}

double CircuitBreakerStatistics::successfulShare() {
	uint64_t currentTimestamp = NS_CURRENT_TIMESTAMP;

	uint64_t retainSince = currentTimestamp - _temporalWindow;

	StatsLock lock;    // Locking the statistics data for exclusive use

	auto statsIt = _stats.begin();
	int successfulCount = 0;
	int totalCount = 0;
	while (statsIt != _stats.end()) {
		if (statsIt->timestamp < retainSince) {
			statsIt = _stats.erase(statsIt);
			continue;
		}

		totalCount++;
		if (statsIt->success) {
			successfulCount++;
		}

		statsIt++;
	}

	if (totalCount == 0) {    // No data in the temporal success table is good
		return 1.0;
	}

	return 1.0 * successfulCount / totalCount;
}

void CircuitBreakerStatistics::registerCallResult(bool success) {
	CallResult callResult;
	callResult.success = success;
	callResult.timestamp = NS_CURRENT_TIMESTAMP;

	StatsLock statsLock;
	_stats.push_back(callResult);
}