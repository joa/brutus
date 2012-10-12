#ifndef BRUTUS_ARENA_H_
#define BRUTUS_ARENA_H_

#include "brutus.h"

namespace brutus {
  namespace internal {
    class Arena {
      public:
        explicit Arena() {}
        void* alloc(size_t size);

        template<class T>
         T* newArray(size_t length);
      private:
        DISALLOW_COPY_AND_ASSIGN(Arena);
    }; //class Arena
  } //namespace internal
} //namespace brutus
#endif
