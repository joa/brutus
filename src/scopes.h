#ifndef _BRUTUS_SCOPES_H
#define _BRUTUS_SCOPES_H

#include <limits>

#include "brutus.h"
#include "arena.h"
#include "name.h"

namespace brutus {
  namespace internal {
    namespace syms {
      class Symbol;

      enum class ScopeKind {
        kUnknown,
        kBlock,
        kClass,
        kFunction,
        kModule
      };

      class Scope : public ArenaMember {
        public:
          static const float DefaultLoadFactor;
          static const int DefaultCapacity = 1 << 2;
          static const int MaximumCapacity = 1 << 30;

          void* operator new(size_t size, Arena* arena) {
            return arena->alloc(size);
          }

          ALWAYS_INLINE ScopeKind kind() {
            return m_kind;
          }

          ALWAYS_INLINE Scope* parent() {
            return m_parent;
          }

          Scope(int initialCapacity, float loadFactor, Arena* arena);
          Scope(Arena* arena);
          ~Scope() {}
          
          // true if it has been added, false otherwise
          bool put(Name* name, Symbol* symbol);
          
          bool put(Symbol* symbol);

          Symbol* putOrOverload(Name* name, Symbol* symbol);
            
          // null if not present
          Symbol* get(Name* name);

          // true if present, false otherwise
          bool contains(Name* name);

          bool contains(Symbol* symbol);

          void init(Scope* parent, ScopeKind kind);

        private:
          void* operator new(size_t size);

          Arena* const m_arena;
          int m_size;
          Symbol** m_table;
          int m_tableSize;
          int m_threshold;
          float m_loadFactor;
          Scope* m_parent;
          ScopeKind m_kind;

          void initTable();
          int hashCodeOf(const char* value, int length);
          void resize(int newCapacity);
          void transfer(Symbol** src, int srcSize, Symbol** dst, int dstSize);

          ALWAYS_INLINE int indexOf(int hash, int length) {
            // Same as "return hash % length" given that length is
            // a power of two.
            int result = hash & (length - 1);

#ifdef DEBUG
            if(result < 0) {
              std::cerr << "Error: NameTable index is less than zero." << std::endl;
            }
#endif

            return result;
          }

          DISALLOW_COPY_AND_ASSIGN(Scope);
      }; //class Scope
    } //namespace sym
  } //namespace internal
} //namespace brutus

#endif