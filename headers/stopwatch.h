#include <chrono>
#include <string>

namespace benchmark {
	class Stopwatch {
		private:
			std::chrono::time_point<std::chrono::high_resolution_clock> lastObservedTimePoint;
			std::chrono::time_point<std::chrono::high_resolution_clock> firstTimePoint;
		public:
			Stopwatch (void);

			void reset (void);
			std::string formatedTotalTime (void);
			std::string formatedLapTime (void);
	};
};
