#include "phases.h"
namespace brutus {
namespace internal {
Phase::Phase(Arena* arena)
    : m_arena(arena) {}

Stopwatch* Phase::stopwatch() {
  return &m_stopwatch;
}

void Phase::log() {
  std::cout << 
    name() << ": " << 
    m_stopwatch.totalMS() << "ms, " << 
    m_stopwatch.totalNS() << "ns" << std::endl;
}
}
}