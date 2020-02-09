//
// Created by dmerkushov on 2/9/20.
//

#ifndef NSCIRCUITBREAKER_H
#define NSCIRCUITBREAKER_H

#include <memory>
#include <vector>

namespace nanoservices {

const std::string CONFIG_PARAMNAME_NSCIRCUITBREAKER_ENABLED("ns-circuitbreaker-enabled");

/**
 * @brief Time window to collect statistics, milliseconds
 */
const std::string CONFIG_PARAMNAME_NSCIRCUITBREAKER_STATISTICS_TIMEWINDOW_MS(
		"ns-circuitbreaker-stats-timewindow-ms"
);

/**
 * @brief No data within the time window is good (true) or bad (false)?
 */
const std::string CONFIG_PARAMNAME_NSCIRCUITBREAKER_STATS_NODATAISGOOD(
		"ns-circuitbreaker-stats-nodataisgood"
);

const std::string CONFIG_PARAMNAME_NSCIRCUITBREAKER_PASS2BROKEN_PERCENTAGE_THRESHOLD(
		"ns-circuitbreaker-pass2broken-percentage-threshold"
);
const std::string CONFIG_PARAMNAME_NSCIRCUITBREAKER_TESTPASS2PASS_PERCENTAGE_THRESHOLD(
		"ns-circuitbreaker-testpass2pass-percentage-threshold"
);

enum CurcuitBreakerState {
	PASS = 0,
	TEST_PASS = 1,
	BROKEN = 2
};

class CallResult {
public:
	uint64_t timestamp;
	bool success;
};

class CircuitBreakerStatistics {
private:
	std::vector<CallResult> _stats;
	uint64_t _window;
public:
	CircuitBreakerStatistics(uint64_t window);

	virtual ~CircuitBreakerStatistics();

	void registerCallResult(bool success);

	double successfulShare();
};
}

#endif //NSCIRCUITBREAKER_H
