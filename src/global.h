#ifndef BRUTUS_GLOBAL_H_
#define BRUTUS_GLOBAL_H_

// Macro to disallow the invocation of copy constructor
// and assignment operator.
#ifdef __GNUC__
  #define DISALLOW_COPY_AND_ASSIGN(T) \
    T& operator=(const T&) = delete; \
    T&& operator=(const T&&) = delete; \
    T(const T&) = delete; \
    T(const T&&) = delete

  #define DISALLOW_CTOR(T) \
    T() = delete
#else
  // VS2012 does not support delete yet so we go with
  // the old version instead.
  #define DISALLOW_COPY_AND_ASSIGN(T) \
    T(const T&); \
    T(const T&&); \
    void operator=(const T&); \
    void operator=(const T&&)

  #define DISALLOW_CTOR(T) \
    T()
#endif

#if _MSC_VER
  // VS2012 does not support unicode literals yet so we
  // noop it for now.
  #define u8
#endif

#define YES true
#define NO false

#ifndef ALWAYS_INLINE
  #ifdef __GNUC__
    #define ALWAYS_INLINE __attribute__((always_inline))
  #else
    #define ALWAYS_INLINE inline
  #endif
#endif

#if _MSC_VER
  //VS2012 does not support constexpr yet so we fallback
  //to the _countof macro.
  #define NumberOfElements(x) _countof(x)
#else
  template<typename T, size_t sizeOfArray>
  constexpr size_t NumberOfElements(T (&)[sizeOfArray]) {
    return sizeOfArray;
  }
#endif

// Preconditions for compiling Brutus
static_assert(sizeof(char) == 1, "sizeof(char) must be one.");

#endif
