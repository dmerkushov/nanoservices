#pragma once

#include <chrono>

namespace nanoservices {
	class NsSkelLoopWorkerInterface {
	public:
		/**
		 * Initialize nanoservice loop worker
		 * @param Timeout between call in milliseconds
		 */
		NsSkelLoopWorkerInterface(int timeout = 1000);
		/**
		 * Disable nanoservice loop worker
		 */
		void disable();
		/**
		 * Call nanoservice loop worker
		 */
		void operator() ();
		/**
		 * User-defined processing for call
		 */
		virtual void process() = 0;
	private:
		std::chrono::milliseconds _timeout;
		std::chrono::time_point<std::chrono::high_resolution_clock> _pp;
		bool _work;
	};
}
