#ifndef BRUTUS_STOPWATCH_H_
#define BRUTUS_STOPWATCH_H_

#include <chrono>

#include "brutus.h"

namespace brutus {
  class Stopwatch {
    public:
      typedef unsigned long long Rep;
      typedef std::chrono::high_resolution_clock Clock;
      typedef std::chrono::duration<Rep , Clock::period> Duration;
      typedef std::chrono::time_point<Clock> TimePoint;

      explicit Stopwatch() {}
      void start();
      void stop();
      void pause(); 
      void resume();
      void log() const;
      void stopAndLog();
      void time(std::function<void()> f);
      template<typename T> T time(std::function<T()> f);
      Duration total();
      Rep totalMS();
      Rep totalNS();

    private:
      static TimePoint now();
      TimePoint m_start;
      Duration m_total;

      DISALLOW_COPY_AND_ASSIGN(Stopwatch);
  }; //class Stopwatch
} //namespace brutus
#endif
