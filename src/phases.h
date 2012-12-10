#ifndef _BRUTUS_PHASES_H
#define _BRUTUS_PHASES_H

#include "brutus.h"
#include "arena.h"
#include "stopwatch.h"
#include "ast.h"
#include "scopes.h"
#include "symbols.h"
#include "lexer.h"
#include "parser.h"

#define PHASE_OVERRIDES() \
  virtual const char* name() override final; \
  virtual void apply(CompilationUnit* unit) override final

namespace brutus {
  class CompilationUnit;

  namespace internal {
    class Phase {
      public:
        explicit Phase(Arena* arena);
        virtual const char* name() = 0;
        virtual void apply(CompilationUnit* unit) = 0;
        Stopwatch* stopwatch();
        void log();

      protected:
        Arena* m_arena;

      private:
        Stopwatch m_stopwatch;
        DISALLOW_COPY_AND_ASSIGN(Phase);
    };

    class ParsePhase : public Phase {
      public:
        explicit ParsePhase(Lexer* lexer, Parser* parser, Arena* arena);
        PHASE_OVERRIDES();

      private:
        Lexer* m_lexer;
        Parser* m_parser;

        DISALLOW_COPY_AND_ASSIGN(ParsePhase);
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
        void buildParameterSymbol(ast::Parameter* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);
        void buildBlockScope(ast::Block* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);
        void buildProgramScope(ast::Program* node, syms::Scope* symbolTable);
        void buildIfSymbol(ast::If* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);
        void buildIfCaseSymbol(ast::IfCase* node, syms::Scope* parentScope, syms::Symbol* parentSymbol);

        DISALLOW_COPY_AND_ASSIGN(SymbolsPhase);
    };

    class LinkPhase : public Phase {
      public:
        explicit LinkPhase(syms::Scope* symbolTable, Arena* arena);
        PHASE_OVERRIDES();

      private:
        syms::Scope* m_symbolTable;
        
        void link(ast::Node* node, syms::Scope* scope, types::Type* parentType);
        syms::Symbol* errorSymbol(syms::Symbol* parent, ast::Node* node, syms::ErrorReason reason);

        DISALLOW_COPY_AND_ASSIGN(LinkPhase);
    };
  }
}
#endif
