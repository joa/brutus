#ifndef _BRUTUS_H_
#define _BRUTUS_H_

// Undefine NULL in favor of nullptr
#ifdef NULL
  #undef NULL
#endif

// Macro to disallow the invocation of copy constructor
// and assignment operator.
#define DISALLOW_COPY_AND_ASSIGN(T) \
  T& operator=(const T&) = delete; \
  T(const T&) = delete;

#endif
