#include "stopwatch.h"

namespace brutus {
void Stopwatch::start() {
  m_start = now();  
}

void Stopwatch::resume() {
  start();
}

void Stopwatch::pause() {
  auto delta = now() - m_start;
  m_total += delta;
}

void Stopwatch::stop() {
  auto delta = now() - m_start;
  m_total += delta;
}

void Stopwatch::log() const {
  auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(m_total);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(m_total);
  std::cout
    << "Elapsed Time: "
    << ms.count()
    << "ms, "
    << ns.count()
    << "ns"
    << std::endl;
}

void Stopwatch::stopAndLog() {
  stop();
  log();
}

TimePoint Stopwatch::now() {
  return std::chrono::high_resolution_clock::now();
}
} //namespace brutus
