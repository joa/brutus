#ifndef BRUTUS_STOPWATCH_H_
#define BRUTUS_STOPWATCH_H_

#include <chrono>

#include "brutus.h"

namespace brutus {
  typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
  typedef std::chrono::duration<unsigned long long, std::chrono::high_resolution_clock::period> Duration;

  class Stopwatch {
    public:
      explicit Stopwatch() : m_start(now()), m_total(0) {}
      void start();
      void stop();
      void pause(); // same as stop
      void resume(); // same as start
      void log() const;
      void stopAndLog();

    private:
      static TimePoint now();
      TimePoint m_start;
      Duration m_total;

      DISALLOW_COPY_AND_ASSIGN(Stopwatch);
  }; //class Stopwatch
} //namespace brutus
#endif
