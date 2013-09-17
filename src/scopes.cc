#include "scopes.h"
#include "symbols.h"

namespace brutus {
namespace internal {
namespace syms {
const float Scope::DefaultLoadFactor = 0.75f;

Scope::Scope(int initialCapacity, float loadFactor, Arena* arena)
    : m_arena(arena),
      m_size(0),
      m_parent(nullptr) {
  int capacity = NextPow2(initialCapacity);
  m_loadFactor = loadFactor;
  m_threshold = static_cast<int>(static_cast<float>(capacity) * loadFactor);
  m_tableSize = capacity;
}

Scope::Scope(Arena* arena)
    : m_arena(arena),
      m_size(0),
      m_parent(nullptr),
      m_kind(ScopeKind::kUnknown) {
  m_loadFactor = DefaultLoadFactor;
  m_threshold = static_cast<int>(static_cast<float>(DefaultCapacity) * DefaultLoadFactor);
  m_tableSize = DefaultCapacity;
}

void Scope::init(Scope* parent, ScopeKind kind) {
  m_parent = parent;
  m_kind = kind;
  initTable();
}

void Scope::initTable() {
  m_table = m_arena->newArray<Symbol*>(m_tableSize);
  ArrayFill(m_table, 0, m_tableSize);
}

bool Scope::contains(Name* name) {
  return get(name) != nullptr;
}

bool Scope::contains(Symbol* symbol) {
  return contains(symbol->name());
}

Symbol* Scope::get(Name* name) {
  const int hashCode = name->m_hashCode;
  auto scope = this;

  do {
    const int keyIndex = indexOf(hashCode, scope->m_tableSize);
    Symbol* entry = scope->m_table[keyIndex];

    while(entry != nullptr) {
      if(entry->m_name == name) {
        return entry;
      }

      entry = entry->m_next;
    }

    scope = scope->m_parent;
  } while(scope != nullptr);

  return nullptr;
}

bool Scope::put(Name* name, Symbol* symbol) {
  const int hashCode = name->m_hashCode;
  const int keyIndex = indexOf(hashCode, m_tableSize);

  Symbol* entry = m_table[keyIndex];

  while(entry != nullptr) {
    if(entry->m_name == name) {
      return false;
    }

    entry = entry->m_next;
  }

  symbol->m_next = m_table[keyIndex];
  m_table[keyIndex] = symbol;

  if(m_size++ >= m_threshold) {
    resize(2 * m_tableSize);
  }

  return true;
}

bool Scope::put(Symbol* symbol) {
  return put(symbol->name(), symbol);
}

Symbol* Scope::putOrOverload(Name* name, Symbol* symbol) {
  const int hashCode = name->m_hashCode;
  const int keyIndex = indexOf(hashCode, m_tableSize);

  Symbol* prev = nullptr;
  Symbol* next = m_table[keyIndex];

  while(next != nullptr) {
    if(next->m_name == name) {
      if(next->kind() == SymbolKind::kOverload) {
        OverloadSymbol* overload = static_cast<OverloadSymbol*>(next);
        overload->add(symbol);
        return overload;
      } else {
        OverloadSymbol* overload = new (m_arena) OverloadSymbol();
        overload->init(name, symbol->parent(), symbol->ast());

        overload->m_next = next->m_next;
        if(prev != nullptr) {
          prev->m_next = overload;
        }

        overload->add(next);
        overload->add(symbol);

        return overload;
      }
    }

    prev = next;
    next = next->m_next;
  }

  symbol->m_next = m_table[keyIndex];
  m_table[keyIndex] = symbol;

  if(m_size++ >= m_threshold) {
    resize(2 * m_tableSize);
  }

  return symbol;
}


void Scope::resize(int newSize) {
  auto oldTable = m_table;
  auto oldSize = m_tableSize;

  if(oldSize == MaximumCapacity) {
    m_threshold = std::numeric_limits<int>::max();
    return;
  }

  Symbol** newTable = m_arena->newArray<Symbol*>(newSize);
  ArrayFill(newTable, 0, newSize);
  transfer(oldTable, oldSize, newTable, newSize);

  m_table = newTable;
  m_tableSize = newSize;
  m_threshold = static_cast<int>(static_cast<float>(newSize) * m_loadFactor);
}

void Scope::transfer(Symbol** src, int srcSize, Symbol** dst, int dstSize) {
  for (int i = 0; i < srcSize; i++) {
    Symbol* symbol = src[i];

    if (symbol != nullptr) {
      do {
        Symbol* next = symbol->m_next;
        int index = indexOf(symbol->name()->m_hashCode, dstSize);

        symbol->m_next = dst[index];
        dst[index] = symbol;

        symbol = next;
      } while (symbol != nullptr);
    }
  }

  ArrayFill(src, 0, srcSize);
}

Symbol* SymbolTable::get(Name* name) {
  // #1 lookup module of name
  // #2 lookup name in module

  //TODO(joa): cache this lookup

  Scope* currentScope = m_scope;
  const char* chars = name->value();
  int32_t length = name->length();
  int32_t lastIndex = 0;

  //TODO(joa): this is sloppy, what if name begins with '.'?

  for(int32_t i = 0; i < length; ++i) {
    if(*(chars + i) == '.') {
      int32_t moduleNameLength = i - lastIndex;

      if(moduleNameLength > 0) {
        auto symbol =
          static_cast<ModuleSymbol*>(
            currentScope->get(
              m_names->get(
                chars + lastIndex,
                moduleNameLength,
                /*copyValue=*/false)));

        if(nullptr == symbol) {
#ifdef DEBUG
          std::cout << "Could not find module: \"";

          for(int32_t j = 0; j < moduleNameLength; ++j) {
            std::cout << *(chars + lastIndex + j);
          }
          std::cout << "\"" << std::endl;
#endif
          return nullptr;
        }

        currentScope = symbol->scope();
        lastIndex = i + 1;
      }
    }
  }

  auto result =
    currentScope->get(m_names->get(chars + lastIndex, length - lastIndex, false));

#ifdef DEBUG
  if(nullptr == result) {
    std::cout << "Could not find \"";
    for(int32_t j = 0; j < length - lastIndex; ++j) {
      std::cout << *(chars + lastIndex + j);
    }
    std::cout << "\"" << std::endl;
  }
#endif

  return result;
}
} //namespace syms
} //namespace internal
} //namespace brutus
