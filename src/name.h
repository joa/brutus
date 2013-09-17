#ifndef BRUTUS_NAME_H_
#define BRUTUS_NAME_H_

#include <limits>

#include "brutus.h"
#include "arena.h"

namespace brutus {
  namespace internal {
    class NameTable;

    namespace syms { class Scope; }

    class Name : public ArenaMember {
      public:
        void* operator new(size_t size, Arena* arena) {
          return arena->alloc(size);
        }

        explicit Name(const int& hashCode, const char* value, const int& length, Name* next)
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
        friend class syms::Scope;

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

        inline Name* empty() {
          return this->get("", 1, /*copyValue=*/false);
        }

        #define PREDEFINED_NAME(x) \
          return this->get(x, sizeof(x) - 1, /*copyValue=*/false)

        inline Name* brutus_Int() { PREDEFINED_NAME("brutus.Int"); }
        inline Name* brutus_String() { PREDEFINED_NAME("brutus.String"); }

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

        void init();
        int hashCodeOf(const char* value, int length);
        void resize(int newCapacity);
        void transfer(Name** src, int srcSize, Name** dst, int dstSize);

        ALWAYS_INLINE int indexOf(int hash, int length) {
          // Same as "return hash % length" given that length is
          // a power of two.
          int result = hash & (length - 1);

#ifdef DEBUG
          if(result < 0) {
            std::cerr << "Error: NameTable index is less than zero." << std::endl;
          }
#endif

          return result;
        }

        DISALLOW_COPY_AND_ASSIGN(NameTable);
    }; //class NameTable
  } //namespace internal
} //namespace brutus

#endif
