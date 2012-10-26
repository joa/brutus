#include "stopwatch.h"

namespace brutus {
void Stopwatch::start() {
  m_total = m_total.zero();
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

Stopwatch::Duration Stopwatch::total() {
  return m_total;
}

Stopwatch::Rep Stopwatch::totalMS() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(m_total).count();
}

Stopwatch::Rep Stopwatch::totalNS() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(m_total).count();
}

Stopwatch::TimePoint Stopwatch::now() {
  return std::chrono::high_resolution_clock::now();
}

template<typename T>
T Stopwatch::time(std::function<T()> f) {
  start();
  T result = f();
  stopAndLog();
  return result;
}

void Stopwatch::time(std::function<void()> f) {
  start();
  f();
  stopAndLog();
}
} //namespace brutus
