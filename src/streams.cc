#include "brutus.h"

namespace brutus {
bool FileCharStream::hasNext() {
  if(m_bufferIndex < m_bufferLength) {
    return true;
  } else {
    updateBuffer();
    return m_bufferLength > 0;
  }
}

char FileCharStream::next() {
  if(m_bufferIndex >= m_bufferLength) {
    updateBuffer();
  }

  auto result = m_buffer[m_bufferIndex];
  ++m_bufferIndex;

  return result;
}

void FileCharStream::updateBuffer() {
  auto numCharsRead =
    fread(
      &m_buffer,
      sizeof(char),
      sizeof(m_buffer),
      const_cast<FILE*>(m_file));

  m_bufferIndex = 0;
  m_bufferLength = numCharsRead;
}

void FileCharStream::foreach(std::function<void(char)> f) {
  while(hasNext()) {
    f(next());
  }
}
}
