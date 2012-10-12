#ifndef BRUTUS_BRUTUS_H_
#define BRUTUS_BRUTUS_H_

#include "../include/brutus.h"
#include "global.h"
#include "alloc.h"

namespace brutus {
  class Arena {
    public:
      explicit Arena() {}
    private:
      DISALLOW_COPY_AND_ASSIGN(Arena);
  }; // class Arena
} // namespace brutus

#endif
