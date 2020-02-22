//
// Created by dmerkushov on 2/9/20.
//

#ifndef NSCIRCUITBREAKER_H
#define NSCIRCUITBREAKER_H

#include <memory>
#include <vector>

#include "NsException.h"

namespace nanoservices {

/**
 * @brief Is the circuit breaker enabled?
 */
const std::string CONFIG_PARAMNAME_NSCIRCUITBREAKER_ENABLED("ns-circuitbreaker-enabled");

/**
 * @brief Default value for "Is the circuit breaker enabled?" - false
 */
const bool CONFIG_DEFAULT_NSCIRCUITBREAKER_ENABLED = false;

/**
 * @brief Time window to collect statistics, milliseconds. Must be >0
 */
const std::string CONFIG_PARAMNAME_NSCIRCUITBREAKER_STATISTICS_TEMPORALWINDOW_MS(
		"ns-circuitbreaker-stats-temporalwindow-ms"
);

/**
 * @brief Default value for the temporal window: 1 minute
 */
const uint64_t CONFIG_DEFAULT_NSCIRCUITBREAKER_STATISTICS_TEMPORALWINDOW_MS = 60000;

/**
 * @brief
 */
const std::string CONFIG_PARAMNAME_NSCIRCUITBREAKER_PASS2BROKEN_PERCENTAGE_THRESHOLD(
		"ns-circuitbreaker-pass2broken-percentage-threshold"
);

const uint8_t CONFIG_DEFAULT_NSCIRCUITBREAKER_PASS2BROKEN_PERCENTAGE_THRESHOLD = 10;

const std::string CONFIG_PARAMNAME_NSCIRCUITBREAKER_TESTPASS2PASS_PERCENTAGE_THRESHOLD(
		"ns-circuitbreaker-testpass2pass-percentage-threshold"
);

enum CurcuitBreakerState {
	PASS = 0,
	TEST_PASS = 1,
	BROKEN = 2
};

struct CallResult {
	/**
	 * @brief Timestamp of the call to the service. Milliseconds from the epoch (1 Jan, 1970)
	 */
	uint64_t timestamp;

	/**
	 * @brief The call is considered successful if no exceptions were thrown by the service processing routine
	 */
	bool success;
};

class CircuitBreakerStatistics {
private:
	/**
	 * @brief Time-series vector of the previous call statistics
	 */
	std::vector<CallResult> _stats;

	/**
	 * @brief Temporal window to retain the call statistics. Milliseconds. Strictly positive.
	 */
	uint64_t _temporalWindow;
public:

	/**
	 * @brief
	 * @param temporalWindow Time temporalWindow to retain the call statistics. Milliseconds. Strictly positive
	 * @throw NsException if a temporalWindow less than or equal to 0 is set
	 */
	explicit CircuitBreakerStatistics(uint64_t temporalWindow) throw(NsException);

	virtual ~CircuitBreakerStatistics();

	void registerCallResult(bool success);

	double successfulShare();
};
}

#endif //NSCIRCUITBREAKER_H
