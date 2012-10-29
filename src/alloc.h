#ifndef BRUTUS_ALLOC_H_
#define BRUTUS_ALLOC_H_

#include <cstdlib> //for std::free and std::malloc
#include <cstring> //for std::memcpy

#include "brutus.h"

namespace brutus {
  namespace internal {
    class Arena;

    template<typename T>
    ALWAYS_INLINE static T* NewArray(const size_t& size) {
      auto result = new T[size];
      
      if(result == nullptr) {
        std::cerr << "Error: Could not allocate array." << std::endl;
      }
      
      return result;
    }

    template<typename T>
    ALWAYS_INLINE static void DeleteArray(T* value) {
      delete[] value;
    }

    // Type-safe array copy operation with no undefined behaviour
    // whether or not target is the same as source.
    template<typename T>
    ALWAYS_INLINE static void ArrayCopy(
        T* target, const T* source, int length) {
      if(target == source) {
        std::memmove(target, source, length);
      } else {
        std::memcpy(target, source, length);
      }
    }

    // Memset for Malloced arrays.
    template<typename T>
    ALWAYS_INLINE static void ArrayFill(T* target, int value, int length) {
      std::memset(
        target,
        value,
        sizeof(T) * length //NOLINT
      );
    }

    class Malloc {
      public:
        ALWAYS_INLINE static void* New(size_t size) {
          auto result = std::malloc(size);

          if(result == nullptr) {
            std::cerr << "Error: Could not allocate object." << std::endl;
          }
          
          return result;
        }

        ALWAYS_INLINE static void Delete(void* ptr) {
          std::free(ptr);
        }
      private:
        DISALLOW_CTOR(Malloc);
        DISALLOW_COPY_AND_ASSIGN(Malloc);
    };

    class Allocator {
      public:
        Allocator() {}
        virtual ~Allocator() {}

        virtual void* alloc(int size) = 0;
        virtual void free(void* ptr) = 0;
        virtual bool freeSupported() const = 0;

        template<typename T>
        ALWAYS_INLINE T* newArray(const size_t& length) {
          return reinterpret_cast<T*>(alloc(sizeof(T) * length));
        }

        template<class T>
        ALWAYS_INLINE T* create() {
          auto ptr = alloc(sizeof(T));
          auto inst = (::new (ptr) T());

          return inst;
        }

      private:
        DISALLOW_COPY_AND_ASSIGN(Allocator);
    };

    class ArenaAllocator : public Allocator {
      public:
        ArenaAllocator(Arena* arena) : m_arena(arena) {}

        void* alloc(int size) override final;
        void free(void* ptr) override final;
        
        bool freeSupported() const override final {
          return NO;
        }
      private:
        Arena* m_arena;
    };

    class HeapAllocator : public Allocator {
      public:
        HeapAllocator() {}

        void* alloc(int size) override final {
          return Malloc::New(size);
        }

        void free(void* ptr) override final {
          Malloc::Delete(ptr);
        }

        bool freeSupported() const override final {
          return YES;
        }
      private:
        DISALLOW_COPY_AND_ASSIGN(HeapAllocator);
    };
  } //namespace internal
} //namespace brutus
#endif
