#include "brutus.h"

namespace brutus {
auto Stopwatch::start() -> void {
  m_start = std::chrono::high_resolution_clock::now();  
}

auto Stopwatch::stop() -> void {
  m_end = std::chrono::high_resolution_clock::now();
}

auto Stopwatch::log() -> void {
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

auto Stopwatch::stopAndLog() -> void {
  stop();
  log();
}
}
