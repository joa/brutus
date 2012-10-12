#ifndef BRUTUS_STOPWATCH_H_
#define BRUTUS_STOPWATCH_H_

#include "brutus.h"
#include <chrono>

namespace brutus {
  namespace internal {
    class Stopwatch {
      public:
        explicit Stopwatch() {}
        void start();
        void stop();
        void log() const;
        void stopAndLog();

    private:
      DISALLOW_COPY_AND_ASSIGN(Stopwatch);
      std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
      std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
    }; //class Stopwatch
  } //namespace internal
} //namespace brutus
#endif
