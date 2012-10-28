#ifndef _BRUTUS_PHASES_H
#define _BRUTUS_PHASES_H

#include "brutus.h"
#include "arena.h"
#include "stopwatch.h"
#include "ast.h"
#include "scopes.h"
#include "symbols.h"

#define PHASE_OVERRIDES() \
  virtual const char* name() override final; \
  virtual void apply(ast::Node* node) override final

namespace brutus {
  namespace internal {
    class Phase {
      public:
        explicit Phase(Arena* arena);
        virtual const char* name() = 0;
        virtual void apply(ast::Node* node) = 0; //TODO(joa): compilation unit!
        Stopwatch* stopwatch();
        void log();

      protected:
        Arena* m_arena;

      private:
        Stopwatch m_stopwatch;
        DISALLOW_COPY_AND_ASSIGN(Phase);
    };

    class SymbolsPhase : public Phase {
      public:
        explicit SymbolsPhase(syms::Scope* symbolTable, Arena* arena);
        PHASE_OVERRIDES();

      private:
        syms::Scope* m_symbolTable;
        
        void buildSymbols(ast::Node* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);
        void buildClassSymbols(ast::Class* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);
        void buildFunctionSymbols(ast::Function* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);
        void buildModuleSymbols(ast::Module* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);
        void buildVariableSymbol(ast::Variable* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);
        void buildBlockScope(ast::Block* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);

        DISALLOW_COPY_AND_ASSIGN(SymbolsPhase);
    };
  }
}
#endif
