#ifndef _BRUTUS_PHASES_H
#define _BRUTUS_PHASES_H

#include "brutus.h"
#include "arena.h"
#include "stopwatch.h"

namespace brutus {
  namespace internal {
    class Phase {
      public:
        explicit Phase(Arena* m_arena);
        virtual const char* name() = 0;
        Stopwatch stopwatch();
      private:
        DISALLOW_COPY_AND_ASSIGN(Phase);
    };
  }
}
#endif
