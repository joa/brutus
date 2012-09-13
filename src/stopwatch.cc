#include "brutus.h"

namespace brutus {
void Stopwatch::start() {
  m_start = std::chrono::high_resolution_clock::now();  
}

void Stopwatch::stop() {
  m_end = std::chrono::high_resolution_clock::now();
}

void Stopwatch::log() const {
  auto deltaTicks = 
    std::chrono::duration<
      double,
      std::chrono::high_resolution_clock::period
    >(m_end - m_start);

  auto deltaMs =
    std::chrono::duration_cast<
      std::chrono::milliseconds
    >(deltaTicks);

  auto deltaNs =
    std::chrono::duration_cast<
      std::chrono::nanoseconds
    >(deltaTicks);

  std::cout
    << u8"Elapsed Time: "
    << deltaMs.count() 
    << u8"ms, " 
    << deltaNs.count() 
    << u8"ns"
    << std::endl;
}

void Stopwatch::stopAndLog() {
  stop();
  log();
}
}
