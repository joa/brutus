#include "name.h"

namespace brutus {
namespace internal {
const float NameTable::DefaultLoadFactor = 0.75f;

NameTable::NameTable(int initialCapacity, float loadFactor, Arena* arena) : m_arena(arena), m_size(0) {
  int capacity = NextPow2(initialCapacity);
  m_loadFactor = loadFactor;
  m_threshold = static_cast<int>(static_cast<float>(capacity) * loadFactor);
  m_tableSize = capacity;
  init();
}

NameTable::NameTable(Arena* arena) : m_arena(arena), m_size(0) {
  m_loadFactor = DefaultLoadFactor;
  m_threshold = static_cast<int>(static_cast<float>(DefaultCapacity) * DefaultLoadFactor);
  m_tableSize = DefaultCapacity;
  init();
}

void NameTable::init() {
  m_table = m_arena->newArray<Name*>(m_tableSize);
  ArrayFill(m_table, 0, m_tableSize);
}

Name* NameTable::get(const char* value, int length, bool copyValue) {
  // First we perform a lookup in the internal
  // hash table which is created by the list of
  // name entries.

  const int hashCode = hashCodeOf(value, length);
  const int keyIndex = indexOf(hashCode, m_tableSize);
  Name* entry = m_table[keyIndex];
  
  while(entry != nullptr) {
    // If the name has an equal hash code and length
    // it is a pretty good candidate.

    if(entry->m_hashCode == hashCode && entry->m_length == length) {
      // The last check requires to test the entire value for
      // equality. Only if this test succeeds we can guarantee
      // that the correct name has been selected.
      bool isEqual = YES;

      const char* p = value;
      const char* q = entry->m_value;

      for(int i = 0; i < length; ++i) {
        if(*p++ != *q++) {
          isEqual = NO;
          break;
        }
      }

      if(isEqual) {
        return entry;
      }
    }

    entry = entry->m_next;
  }

  // No existing Name was found. We need to insert a new
  // entry into the table. We also know for sure that no
  // such key->value association exists.

  if(copyValue) {
    // The Lexer shares the same buffer to save some space.
    // Copying the value into a name is the only operation
    // we perform to persist it.
    //
    // That is where copyValue comes into place. If one might
    // want to enter a name manually it is absolutely possible
    // to specify this and no additional copy is made.
    auto valueCopy = m_arena->newArray<char>(length);
    ArrayCopy(valueCopy, value, kCharSize * length);
    value = valueCopy;
  }
  
  auto oldName = m_table[keyIndex];
  auto newName = new (m_arena) Name(hashCode, value, length, oldName);

  m_table[keyIndex] = newName;

  if(m_size++ >= m_threshold) {
    resize(2 * m_tableSize);
  }

  return newName;
}

int NameTable::hashCodeOf(const char* value, int length) {
  const char* p = value;
  unsigned int sum = 23;
  
  for(int i = 0; i < length; ++i) {
    sum += (*p++ * 31);
  }

  sum ^= (sum >> 20) ^ (sum >> 12);
  sum ^= (sum >>  7) ^ (sum >>  4);

  return static_cast<int>(sum);
}

void NameTable::resize(int newSize) {
  auto oldTable = m_table;
  auto oldSize = m_tableSize;

  if(oldSize == MaximumCapacity) {
    m_threshold = std::numeric_limits<int>::max();
    return;
  }

  Name** newTable = m_arena->newArray<Name*>(newSize);
  ArrayFill(newTable, 0, newSize);
  transfer(oldTable, oldSize, newTable, newSize);

  m_table = newTable;
  m_tableSize = newSize;
  m_threshold = static_cast<int>(static_cast<float>(newSize) * m_loadFactor);
}

void NameTable::transfer(Name** src, int srcSize, Name** dst, int dstSize) {
  for (int i = 0; i < srcSize; i++) {
    Name* name = src[i];

    if (name != nullptr) {
      do {
        Name* next = name->m_next;
        int index = indexOf(name->m_hashCode, dstSize);

        name->m_next = dst[index];
        dst[index] = name;

        name = next;
      } while (name != nullptr);
    }
  }

  ArrayFill(src, 0, srcSize);
}
} //namespace internal
} //namespace brutus