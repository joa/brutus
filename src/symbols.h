#ifndef _BRUTUS_SYMBOLS_H
#define _BRUTUS_SYMBOLS_H

#include "brutus.h"
#include "arena.h"
#include "ast.h"
#include "name.h"
#include "scopes.h"

#define SYMBOL_OVERRIDES() \
  SymbolKind kind() const override final

namespace brutus {
  namespace internal {
    namespace types {
      class Type;
    }

    namespace syms {
      enum class SymbolKind {        
        kClass,
        kError,
        kFunction,
        kModule,
        kOverload,
        kVariable,
        kEmpty
      }; //enum SymbolKind

      enum class ErrorReason {
        kUnknown,
        kNoSuchName
      };
      
      class OverloadSymbol;

      class Symbol : public ArenaMember {
        public:
          void* operator new(size_t size, Arena* arena) {
            return arena->alloc(size);
          }
          
          ALWAYS_INLINE Name* name() const {
            return m_name;
          }

          ALWAYS_INLINE Symbol* parent() const {
            return m_parent;
          }
          
          ALWAYS_INLINE ast::Node* ast() const {
            return m_ast;
          }

          ALWAYS_INLINE types::Type* type() const {
            return m_type;
          }

          ALWAYS_INLINE void type(types::Type* value) {
            m_type = value;
          }

          explicit Symbol() 
              : m_name(nullptr), 
                m_parent(nullptr), 
                m_ast(nullptr), 
                m_next(nullptr) {}

          virtual ~Symbol() {}
          virtual SymbolKind kind() const = 0;

        protected:
          ALWAYS_INLINE void init(Name* name, Symbol* parent, ast::Node* ast, types::Type* type) {
            m_name = name;
            m_parent = parent;
            m_ast = ast;
            m_type = type;
          }

          Name* m_name;
          Symbol* m_parent;
          ast::Node* m_ast;
          types::Type* m_type;
          //used?
          //flags?
         
        private:
          Symbol* m_next;
          void* operator new(size_t size);

          // for access to m_next
          friend class Scope;
          friend class OverloadSymbol;

          DISALLOW_COPY_AND_ASSIGN(Symbol);
      }; //class Symbol

      template<SymbolKind K>
      class DeclarativeSymbol : public Symbol {
        public:
          DeclarativeSymbol() : m_scope(nullptr) {}
          
          void init(Name* name, Symbol* parent, ast::Node* ast, Scope* scope, types::Type* type) {
            Symbol::init(name, parent, ast, type);
            m_scope = scope;
          }
          
          Scope* scope() const {
            return m_scope;
          }

          SymbolKind kind() const override final {
            return K;
          }

        private:
          Scope* m_scope;

          DISALLOW_COPY_AND_ASSIGN(DeclarativeSymbol);
      }; //class DeclarativeSymbol

      // Because ClassSymbol, FunctionSymbol and ModuleSymbol all share
      // the same code (currently), we forward their type to DeclarativeSymbol<K>
      // with K being the kind of the symbol.
      //
      // This is useful because we can make ClassSymbol its own class in
      // the future and we do not pay the price for inheriting from
      // some stupid base class just to save a couple lines of code.
      typedef DeclarativeSymbol<SymbolKind::kClass> ClassSymbol;     
      typedef DeclarativeSymbol<SymbolKind::kFunction> FunctionSymbol;
      typedef DeclarativeSymbol<SymbolKind::kModule> ModuleSymbol;     

      class ErrorSymbol : public Symbol {
        public:
          ErrorSymbol();
          void init(Name* name, Symbol* parent, ast::Node* ast, ErrorReason reason);
          ErrorReason reason() const;
          SYMBOL_OVERRIDES();

        private:
          ErrorReason m_reason;

          DISALLOW_COPY_AND_ASSIGN(ErrorSymbol);
      }; //class ErrorSymbol

      class OverloadSymbol : public Symbol {
        public:
          OverloadSymbol();
          void init(Name* name, Symbol* parent, ast::Node* ast);
          void add(Symbol* symbol);
          SYMBOL_OVERRIDES();

        private:
          Symbol* m_first;

          DISALLOW_COPY_AND_ASSIGN(OverloadSymbol);
      }; //class OverloadSymbol

      class VariableSymbol : public Symbol {
        public:
          VariableSymbol();
          void init(Name* name, Symbol* parent, ast::Node* ast);
          SYMBOL_OVERRIDES();

        private:
          DISALLOW_COPY_AND_ASSIGN(VariableSymbol);
      }; //class VariableSymbol

      class EmptySymbol : public Symbol {
        public:
          EmptySymbol();
          void init(Symbol* parent, ast::Node* ast);
          SYMBOL_OVERRIDES();

        private:
          DISALLOW_COPY_AND_ASSIGN(EmptySymbol);
      }; //class EmptySymbol
    } //namespace syms
  } //namespace internal
} //namespace brutus

#endif
