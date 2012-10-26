#ifndef _BRUTUS_SYMBOLS_H
#define _BRUTUS_SYMBOLS_H

#include "brutus.h"
#include "arena.h"
#include "ast.h"

namespace brutus {
  namespace internal {
    namespace syms {
      enum Kind {
        CLASS,
        ERROR,
        FUNCTION,
        MODULE,
        VARIABLE
      }; //enum Kind

      class Symbol : public ArenaMember {
        public:
          void* operator new(size_t size, Arena* arena) {
            return arena->alloc(size);
          }
          
          ALWAYS_INLINE Symbol* parent() const {
            return m_parent;
          }
          
          ALWAYS_INLINE ast::Node* ast() const {
            return m_ast;
          }

          Symbol() : m_parent(nullptr), m_ast(nullptr) {}
          virtual ~Symbol() {}
          virtual Kind kind() const = 0;

        protected:
          ALWAYS_INLINE void init(Symbol* parent, ast::Node* ast) {
            m_parent = parent;
            m_ast = ast;
          }

          Symbol* m_parent;
          ast::Node* m_ast;
          //type?
          //used?
          //flags?

        private:
          void* operator new(size_t size);

          DISALLOW_COPY_AND_ASSIGN(Symbol);
      };

      class ClassSymbol : public Symbol {
        public:
          ClassSymbol();

        private:
          DISALLOW_COPY_AND_ASSIGN(ClassSymbol);
      };
    } //namespace syms
  } //namespace internal
} //namespace brutus

#endif
