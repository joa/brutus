#ifndef BRUTUS_GLOBAL_H_
#define BRUTUS_GLOBAL_H_

#if defined(__GNUC__) && __GNUC__ < 4
#error "Building Brutus on Linux requires a GCC 4.x compatible compiler."
#endif

#if defined(_MSC_VER) && _MSC_VER < 1700
#error "Building Brutus on Windows requires at least Visual Studio 2012."
#endif

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

// More intuitive names for boolean literals
#define YES true
#define NO false

#ifdef __GNUC__
  #define ALWAYS_INLINE __attribute__((always_inline))
#else
  #define ALWAYS_INLINE __forceinline
#endif

#ifdef __GNUC__
  #define MUST_USE_RESULT __attribute__ ((warn_unused_result))
#else
  #define MUST_USE_RESULT
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

#define UNUSED(x) \
  ((void)(x))

#define IS_POW_2(x) \
  (((x) > 1) && (0 == ((x) & ((x) - 1))))

#define ASSERT_POW2(x) \
  static_assert(IS_POW_2(x), "Error: " #x " must be a power of two.")

ALWAYS_INLINE int NextPow2(int x) {
  --x;
	x |= x >> 0x01;
	x |= x >> 0x02;
	x |= x >> 0x04;
	x |= x >> 0x08;
	x |= x >> 0x10;
	++x;

	return x;
}
#endif
