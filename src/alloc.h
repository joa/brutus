#ifndef BRUTUS_ALLOC_H_
#define BRUTUS_ALLOC_H_

#include <cstdlib> //for std::free and std::malloc
#include <cstring> //for std::memcpy

#include "brutus.h"

namespace brutus {
  namespace internal {
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
  } //namespace internal
} //namespace brutus
#endif
