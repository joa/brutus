#ifndef BRUTUS_ALLOC_H_
#define BRUTUS_ALLOC_H_

namespace brutus {
  namespace internal {
    template<typename T>
    ALWAYS_INLINE T* NewArray(const size_t& size) {
      auto result = new T[size];
      if(result == nullptr) {
        std::cerr << "Error: Could not allocate array.";
      }
      return result;
    }

    template<typename T>
    ALWAYS_INLINE void DeleteArray(T* value) {
      delete[] value;
    }
  } //namespace internal
} //namespace brutus
#endif
