#include "brutus.h"

namespace brutus {
auto FileCharStream::hasNext() -> bool {
  if(m_bufferIndex < m_bufferLength) {
    return YES;
  } else {
    updateBuffer();
    return m_bufferLength > 0;
  }
}

auto FileCharStream::next() -> char {
  if(m_bufferIndex >= m_bufferLength) {
    updateBuffer();
  }

  auto result = m_buffer[m_bufferIndex];
  ++m_bufferIndex;

  return result;
}

auto FileCharStream::updateBuffer() -> void {
  auto numCharsRead =
    fread(
      &m_buffer,
      sizeof(char),
      sizeof(m_buffer),
      const_cast<FILE*>(m_file));

  m_bufferIndex = 0;
  m_bufferLength = numCharsRead;
}

auto FileCharStream::foreach(std::function<void(char)> f) -> void {
  while(hasNext()) {
    f(next());
  }
}
}
