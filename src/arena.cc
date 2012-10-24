#include "arena.h"

namespace brutus {
namespace internal {
void Arena::init() {
#ifdef DEBUG
  if(m_alignment < 2 || 0 != (m_alignment & (m_alignment - 1))) {
    std::cerr 
      << "Error: Given alignment "
      << m_alignment 
      << " is not a power of two." << std::endl;
  }
#endif

  // In the initialization we assume that the current NIL block
  // is exhausted. That way the grow() function is required
  // to fill an entire block instead of assuming the position is
  // 0 and we are at the beginning of an unused block.
  m_position = aligned(m_blockSize);

  // Grow to the initial capacity.
  grow(m_initialCapacity);

  // The current block is the first free block.
  // We remove the current block of the free list.
  nextBlock();
}

Arena::~Arena() {
  deleteAll();
}

void* Arena::alloc(int size) {
  auto alignedSize = aligned(size);
  auto blockSize = aligned(m_blockSize);

  if(alignedSize > blockSize) {
    //TODO(joa): could allocate an extra block of aligned size, add to 
    // used and return its offset
    std::cerr << "Cannot allocate " << size << " bytes at once.";
    return nullptr;
  }

  auto availableInCurrentBlock = blockSize - m_position;

  if(alignedSize > availableInCurrentBlock) {
    grow(m_blockSize);
    nextBlock();
  }

  auto addr = m_currentBlock->offset(m_position);
  m_position += alignedSize;

#ifdef DEBUG
  if(m_position > blockSize) {
    std::cerr << 
      "Error: Internal error in area. "
      "Consumed more memory than block size allows."
      << std::endl;
  }
#endif

  return addr;
}

void Arena::nextBlock() {
  if(m_currentBlock != nullptr) {
    // Move the old block into the used-list.
    m_currentBlock->next(m_usedBlocks);
    m_usedBlocks = m_currentBlock;
  }

  // Accquire a new block from the free-list.
  m_currentBlock = m_freeBlocks;

  if(m_currentBlock != nullptr) {
    m_freeBlocks = m_currentBlock->next();
    m_currentBlock->next(nullptr);
    m_position = 0;
  } else {
    // There was no block in the free-list and this is
    // an internal error in the arena.
    std::cerr << "Warning: Tried to advance to "
      "a non-existing block." << std::endl;
  }
}

void Arena::grow(int size) {
#ifdef DEBUG
  std::cout << "Growing arena by " << size << "byte(s)" << std::endl;
#endif

  // Grow to the required size using as many blocks as
  // we need. For instance if the block size is 512 and we
  // need to grow by 1025 bytes we allocate three blocks.
  //
  // However there might be an already existing block that
  // still has 256 bytes spare for instance so we would need
  // to allocate only two blocks.

  auto blockSize = aligned(m_blockSize);
  auto available = blockSize - m_position;
  auto required = aligned(size) - available;

  while(required > 0) {
    m_freeBlocks = new ArenaBlock(blockSize, m_freeBlocks);
    ++m_totalBlocks;
    m_totalBytes += blockSize;
    required -= blockSize;
  }
}

void Arena::deleteAll() {
  if(m_currentBlock != nullptr) {
    delete m_currentBlock;
    m_currentBlock = nullptr;
  }

  if(m_freeBlocks != nullptr) {
    auto p = m_freeBlocks;

    while(p != nullptr) {
      auto q = p;
      p = p->next();
      delete q;
    }

    m_freeBlocks = nullptr;
  }

  if(m_usedBlocks != nullptr) {
    auto p = m_usedBlocks;

    while(p != nullptr) {
      auto q = p;
      p = p->next();
      delete q;
    }

    m_usedBlocks = nullptr;
  }

  m_totalBlocks = 0;
  m_totalBytes = 0;
  m_position = aligned(m_blockSize);
}
} //namespace internal
} //namespace brutus
