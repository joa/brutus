#ifndef BRUTUS_NAME_H_
#define BRUTUS_NAME_H_

#include <limits>

#include "brutus.h"
#include "arena.h"

namespace brutus {
  namespace internal {  
    class NameTable;

    class Name : public ArenaMember {
      public:
        void* operator new(size_t size, Arena* arena) {
          return arena->alloc(size);
        }

        Name(const int& hashCode, const char* value, const int& length, Name* next)
            : m_hashCode(hashCode),
              m_value(value),
              m_length(length),
              m_next(next) {}

        ALWAYS_INLINE const char* value() const {
          return m_value;
        }

        ALWAYS_INLINE int length() const {
          return m_length;
        }
      private:
        void* operator new(size_t size);
        const int m_hashCode;
        const char* m_value;
        const int m_length;
        Name* m_next;

        friend class NameTable;

        DISALLOW_COPY_AND_ASSIGN(Name);
    }; //class Name
    
    class NameTable {
      public:
        static const float DefaultLoadFactor;
        static const int DefaultCapacity = 1 << 8;
        static const int MaximumCapacity = 1 << 30;

        explicit NameTable(int initialCapacity, float loadFactor, Arena* arena);
        explicit NameTable(Arena* arena);
        Name* get(const char* value, int length, bool copyValue);

        inline int size() { 
          return m_size;
        }
      private:
        Arena* const m_arena;
        int m_size;
        Name** m_table;
        int m_tableSize;
        int m_threshold;
        float m_loadFactor;

        int hashCodeOf(const char* value, int length);
        void resize(int newCapacity);
        void transfer(Name** src, int srcSize, Name** dst, int dstSize);

        ALWAYS_INLINE int indexOf(int hash, int length) {
          // Same as "return hash % length" given that length is
          // a power of two.
          return hash & (length - 1);
        }

        DISALLOW_COPY_AND_ASSIGN(NameTable);
    }; //class NameTable
  } //namespace internal
} //namespace brutus

#endif
