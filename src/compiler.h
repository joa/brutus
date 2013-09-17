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
  }; //enum SourceKind

  class Source {
    public:
      Source() {}
      virtual ~Source() {}
      virtual SourceKind kind() const = 0;
      virtual CharStream* newStream() const = 0;

    private:
      DISALLOW_COPY_AND_ASSIGN(Source);
  }; //class Source

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
  }; //class FileSource

  class CompilationInfo {
    public:
      explicit CompilationInfo();
      int totalErrors();
      int totalWarnings();

    private:
      DISALLOW_COPY_AND_ASSIGN(CompilationInfo);
  }; //class CompilationInfo

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
  }; //class CompilationUnit

  class Context {
    public:
      Context() {}
      virtual ~Context() {}
      virtual internal::Arena* arena() = 0;
      virtual internal::ArenaAllocator* allocator() = 0;
      virtual internal::NameTable* names() = 0;
      virtual internal::syms::SymbolTable* symbols() = 0;

    private:
      DISALLOW_COPY_AND_ASSIGN(Context);
  }; //class Context

  class Compiler : Context {
    public:
      Compiler();
      ~Compiler();

      void addSource(FILE* fp);
      void compile();

      internal::Arena* arena() override final {
        return m_arena;
      }

      internal::ArenaAllocator* allocator() override final {
        return m_arenaAlloc;
      }

      internal::NameTable* names() override final {
        return m_names;
      }

      internal::syms::SymbolTable* symbols() override final {
        return m_symbolTable;
      }
    private:
      internal::Arena* m_arena;
      internal::ArenaAllocator* m_arenaAlloc;
      internal::NameTable* m_names;
      internal::syms::SymbolTable* m_symbolTable;
      List<internal::Phase*>* m_phases;
      List<CompilationUnit*>* m_units;
      int m_phase;
      DISALLOW_COPY_AND_ASSIGN(Compiler);
  }; //class Compiler
} //namespace brutus

#endif
