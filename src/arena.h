#ifndef BRUTUS_ARENA_H_
#define BRUTUS_ARENA_H_

#include "brutus.h"

namespace brutus {
  namespace internal {
    class Arena {
      public:
        Arena(int initialCapacity, int blockSize, int alignment) : 
          m_initialCapacity(initialCapacity),
          m_blockSize(blockSize),
          m_alignment(alignment),
          m_position(0),
          m_totalBlocks(0),
          m_totalBytes(0),
          m_currentBlock(nullptr),
          m_freeBlocks(nullptr),
          m_usedBlocks(nullptr) {}

        ~Arena();

        void init();

        void* alloc(int size);
 
        template<typename T>
        ALWAYS_INLINE T* newArray(const size_t& length) {
          return reinterpret_cast<T*>(alloc(sizeof(T) * length));
        }

        void deleteAll();

      private:
        DISALLOW_COPY_AND_ASSIGN(Arena);

        class ArenaBlock {
          #define SIZE_OF_BYTE sizeof(char)

          public:
            explicit ArenaBlock(int size, ArenaBlock* next) : m_next(next) {
              m_block = reinterpret_cast<char*>(Malloc::New(size * SIZE_OF_BYTE));
            }

            ~ArenaBlock() {
              Malloc::Delete(m_block);
              m_next = nullptr;
            }

            ALWAYS_INLINE ArenaBlock* next() const {
              return m_next;
            }

            ALWAYS_INLINE void next(ArenaBlock* next) {
              m_next = next;
            }

            ALWAYS_INLINE char* offset(int size) const {
              return m_block + size * SIZE_OF_BYTE;
            }

          private:
            DISALLOW_COPY_AND_ASSIGN(ArenaBlock);
            ArenaBlock* m_next;
            char* m_block;

          #undef SIZE_OF_BYTE
        };

        void grow(int size);
        void nextBlock();

        ALWAYS_INLINE int aligned(int size) {
          //padding = (alignment - (size % align)) % alignment
          return (size + m_alignment - 1) & ~(m_alignment - 1);
        }

        const int m_initialCapacity;
        const int m_blockSize;
        const int m_alignment;
        int m_position;
        int m_totalBlocks;
        int m_totalBytes;
        ArenaBlock* m_currentBlock;
        ArenaBlock* m_freeBlocks;
        ArenaBlock* m_usedBlocks;
    }; //class Arena

    class ArenaMember {
      public:
        ALWAYS_INLINE void* operator new(size_t size, Arena* arena);
        void operator delete(void*, size_t) { /*error*/ }
        void operator delete(void* p, Arena* arena) {
          (void)p;
          (void)arena;
          /*error*/
        }
    }; //class ArenaMember
  } //namespace internal
} //namespace brutus
#endif
