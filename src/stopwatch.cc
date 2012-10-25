#include "stopwatch.h"

namespace brutus {
namespace internal {
void Stopwatch::start() {
  m_start = std::chrono::high_resolution_clock::now();  
}

void Stopwatch::stop() {
  m_end = std::chrono::high_resolution_clock::now();
}

void Stopwatch::log() const {
#if __GNUC__
  //TODO(joa): deltaTicks cannot be defined with VS2012 and leads to an error.
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
    << "Elapsed Time: "
    << deltaMs.count() 
    << "ms, " 
    << deltaNs.count() 
    << "ns"
    << std::endl;
#else
  auto deltaTicks = m_end - m_start;

  auto deltaNs =
    std::chrono::duration_cast<
      std::chrono::nanoseconds
    >(deltaTicks);

  std::cout
    << "Elapsed Time: "
    << (deltaNs.count() / 1000000)
    << "ms, "
    << deltaNs.count() 
    << "ns"
    << std::endl;
#endif
}

void Stopwatch::stopAndLog() {
  stop();
  log();
}
} //namespace internal
} //namespace brutus
