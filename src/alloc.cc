#include "alloc.h"

#include "arena.h"

namespace brutus {
namespace internal {
void* ArenaAllocator::alloc(int size) {
  return m_arena->alloc(size);
}

void ArenaAllocator::free(void* ptr) {
  UNUSED(ptr);
}
}
}
