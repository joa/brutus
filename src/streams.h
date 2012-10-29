#ifndef BRUTUS_STREAMS_H_
#define BRUTUS_STREAMS_H_

#include "brutus.h"

namespace brutus {
  class CharStream {
    public:
      explicit CharStream() {}
      virtual ~CharStream() {}
      virtual bool hasNext() = 0;
      virtual char next() = 0;
      virtual void foreach(std::function<void(char)> f) = 0; //NOLINT

    private:
      DISALLOW_COPY_AND_ASSIGN(CharStream);
  }; // class CharStream

  namespace internal {
    class FileCharStream : public CharStream {
      public:
        explicit FileCharStream(FILE* file) : 
          m_file(file),
          m_bufferIndex(0),
          m_bufferLength(0) {}
        bool hasNext();
        char next();
        void foreach(std::function<void(char)> f); //NOLINT

      private:
        const FILE* m_file;
        char m_buffer[0x1000];
        size_t m_bufferIndex;
        size_t m_bufferLength;

        void updateBuffer();

        DISALLOW_COPY_AND_ASSIGN(FileCharStream);
    }; // class FileCharStream
  } //namespace internal
} //namespace brutus
#endif
