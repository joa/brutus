#ifndef BRUTUS_ALLOC_H_
#define BRUTUS_ALLOC_H_

#include "brutus.h"
#include <cstdlib> //for std::free and std::malloc
#include <cstring> //for std::memcpy

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
    ALWAYS_INLINE static void ArrayCopy(T* target, const T* source, size_t length) {
      if(target == source) {
        std::memmove(target, source, length);
      } else {
        std::memcpy(target, source, length);
      }
    }
#if 0
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
#endif
  } //namespace internal
} //namespace brutus
#endif
