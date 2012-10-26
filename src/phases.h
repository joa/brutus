#ifndef _BRUTUS_PHASES_H
#define _BRUTUS_PHASES_H

#include "brutus.h"
#include "arena.h"
#include "stopwatch.h"
#include "ast.h"

namespace brutus {
  namespace internal {
    class Phase {
      public:
        explicit Phase(Arena* arena);
        virtual const char* name() = 0;
        virtual void apply(ast::Node* node) = 0; //TODO(joa): compilation unit!
        Stopwatch* stopwatch();
        void log();

      private:
        Arena* m_arena;
        Stopwatch m_stopwatch;
        DISALLOW_COPY_AND_ASSIGN(Phase);
    };
  }
}
#endif
