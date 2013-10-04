#ifndef BRUTUS_NAME_H_
#define BRUTUS_NAME_H_

#include <limits>

#include "brutus.h"
#include "arena.h"

namespace brutus {
  namespace internal {
    class NameTable;

    template<class T>
    class NameCache;
    
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
        friend class NameCache<void>;
        friend class syms::Scope;

        DISALLOW_COPY_AND_ASSIGN(Name);
    }; //class Name
    
    //TODO(joa): can be replaced with generic map<K,V>
    template<class T>
    class NameCache {
      private:
        template<class T>
        class Entry : public ArenaMember {
          public:
            void* operator new(size_t size, Arena* arena) {
              return arena->alloc(size);
            }

            explicit Entry(Name* key, T value, Entry<T>* next) 
                : m_key(key), 
                  m_value(value),
                  m_next(next) {}

          private:
            Name* m_key;
            T m_value;
            Entry<T>* m_next;
        };

      public:
        explicit NameCache(int initialCapacity, float loadFactor, Arena* arena) 
            : m_arena(arena) {
          int capacity = NextPow2(initialCapacity);
          m_size = 0;
          m_loadFactor = loadFactor;
          m_threshold = static_cast<int>(static_cast<float>(capacity) * loadFactor);
          m_tableSize = capacity;
          init();
        }

        explicit NameCache(Arena* arena) 
            : m_arena(arena) {
          m_size = 0;
          m_loadFactor = NameTable::DefaultLoadFactor;
          m_threshold = static_cast<int>(static_cast<float>(NameTable::DefaultCapacity) * NameTable::DefaultLoadFactor);
          m_tableSize = NameTable::DefaultCapacity;
          init();
        }

        T get(Name* key);
        void put(Name* key, T value);
        bool contains(Name* key);

      private:
        Arena* const m_arena;
        int m_size;
        Entry<T>** m_table;
        int m_tableSize;
        int m_threshold;
        float m_loadFactor;

        void init();
        int hashCodeOf(Name* value);
        void resize(int newCapacity);
        void transfer(Entry<T>** src, int srcSize, Entry<T>** dst, int dstSize);

        ALWAYS_INLINE int indexOf(int hash, int length) {
          // Same as "return hash % length" given that length is
          // a power of two.
          int result = hash & (length - 1);

#ifdef DEBUG
          if(result < 0) {
            std::cerr << "Error: NameCache index is less than zero." << std::endl;
          }
#endif

          return result;
        }

        DISALLOW_COPY_AND_ASSIGN(NameCache);
    };

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
