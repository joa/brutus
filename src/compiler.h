#ifndef _BRUTUS_COMPILER_H
#define _BRUTUS_COMPILER_H

#include "brutus.h"
#include "arena.h"
#include "name.h"
#include "lexer.h"
#include "parser.h"
#include "list.h"
#include "phases.h"
#include "streams.h"
#include "ast.h"

namespace brutus {
  enum class SourceKind {
    kFile
  };

  class Source {
    public:
      Source() {}
      virtual ~Source() {}
      virtual SourceKind kind() const = 0;
      virtual CharStream* newStream() const = 0;

    private:
      DISALLOW_COPY_AND_ASSIGN(Source);
  };

  class FileSource : public Source {
    public:
      FileSource(FILE* fp) : m_fp(fp) {}
      
      SourceKind kind() const override final {
        return SourceKind::kFile;
      }

      CharStream* newStream() const override final {
        return new brutus::internal::FileCharStream(m_fp);
      }

    private:
      FILE* m_fp;

      DISALLOW_COPY_AND_ASSIGN(FileSource);
  };

  class CompilationInfo {
    public:
      explicit CompilationInfo();
      int totalErrors();
      int totalWarnings();

    private:
      DISALLOW_COPY_AND_ASSIGN(CompilationInfo);
  };

  class CompilationUnit {
    public:
      explicit CompilationUnit() 
          : m_ast(nullptr), 
            m_source(nullptr) {}

      ~CompilationUnit() {
        // A compilation unit is responsible for deleting
        // the source associated with it. It is a mere
        // container for the AST and the source.
        //
        // However the AST does not have to be deleted
        // since it is allocated within the arena of the 
        // compiler.
        //
        // If the compiler needs to compile the same unit
        // twice only the whole AST will be gone, not the
        // source.
        delete m_source;
      }

      internal::ast::Node* ast() const;
      void ast(internal::ast::Node* value);

      Source* source() const;
      void source(Source* value);

    private:
      internal::ast::Node* m_ast;
      Source* m_source;

      DISALLOW_COPY_AND_ASSIGN(CompilationUnit);
  };

  class Compiler {
    public:
      Compiler();
      ~Compiler();

      void addSource(FILE* fp);
      void compile();
    private:
      internal::Arena* m_arena;
      internal::ArenaAllocator* m_arenaAlloc;
      internal::NameTable* m_names;
      internal::syms::Scope* m_symbolTable;
      List<internal::Phase*>* m_phases;
      List<CompilationUnit*>* m_units;
      internal::Lexer* m_lexer;
      internal::Parser* m_parser;
      int m_phase;
      DISALLOW_COPY_AND_ASSIGN(Compiler);
  };
} //namespace brutus

#endif
