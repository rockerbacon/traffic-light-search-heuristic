#pragma once

#include <chrono>
#include <sstream>

namespace benchmark {
	
	class Stopwatch {
		private:
			std::chrono::time_point<std::chrono::high_resolution_clock> lapBegin;
			std::chrono::time_point<std::chrono::high_resolution_clock> stopwatchBegin;
		public:
			Stopwatch (void);

			void reset (void);
			std::string formatedTotalTime (void) const;
			std::string formatedLapTime (void) const;
			void newLap (void);
	};

	std::ostream& operator<< (std::ostream &stream, std::chrono::high_resolution_clock::duration duration);

}
