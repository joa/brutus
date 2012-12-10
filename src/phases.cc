#include "phases.h"
#include "compiler.h"
#include "types.h"

namespace brutus {
namespace internal {
Phase::Phase(Arena* arena)
    : m_arena(arena) {}

Stopwatch* Phase::stopwatch() {
  return &m_stopwatch;
}

void Phase::log() {
  std::cout << 
    name() << ": " << 
    m_stopwatch.totalMS() << "ms, " << 
    m_stopwatch.totalNS() << "ns" << std::endl;
}

//

ParsePhase::ParsePhase(Lexer* lexer, Parser* parser, Arena* arena)
    : Phase(arena), 
      m_lexer(lexer),
      m_parser(parser) {}

const char* ParsePhase::name() {
  return "ParsePhase";
}

void ParsePhase::apply(CompilationUnit* unit) {
  auto source = unit->source();

  switch(source->kind()) {
    case SourceKind::kFile: {
        auto stream = unit->source()->newStream();
  
        m_lexer->init(stream);
        unit->ast(m_parser->parseProgram());

        auto printer = new brutus::internal::ast::ASTPrinter(std::cout);     
        printer->print(unit->ast());

        delete stream;
      }
      break;
#ifdef DEBUG
    default:
      std::cerr << "Unexpected source: " << static_cast<int>(source->kind()) << std::endl;
#endif
  }
}

//

SymbolsPhase::SymbolsPhase(syms::Scope* symbolTable, Arena* arena)
    : Phase(arena), 
      m_symbolTable(symbolTable) {}

const char* SymbolsPhase::name() {
  return "SymbolsPhase";
}

void SymbolsPhase::apply(CompilationUnit* unit) {
  buildSymbols(unit->ast(), m_symbolTable, nullptr);
}

void SymbolsPhase::buildSymbols(ast::Node* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
#ifdef DEBUG
  if(nullptr == node) {
    std::cerr << "Node is null." << std::endl;
    return;
  }
#endif

  switch(node->kind()) {
    case ast::NodeKind::kBlock:
      buildBlockScope(static_cast<ast::Block*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kClass:
      buildClassSymbols(static_cast<ast::Class*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kFunction:
      buildFunctionSymbols(static_cast<ast::Function*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kModule:
      buildModuleSymbols(static_cast<ast::Module*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kParameter:
      buildParameterSymbol(static_cast<ast::Parameter*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kProgram:
      buildProgramScope(static_cast<ast::Program*>(node), parentScope);
      break;

    case ast::NodeKind::kVariable:
      buildVariableSymbol(static_cast<ast::Variable*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kIf:
      buildIfSymbol(static_cast<ast::If*>(node), parentScope, parentSymbol);
      break;
    case ast::NodeKind::kIfCase:
      buildIfCaseSymbol(static_cast<ast::IfCase*>(node), parentScope, parentSymbol);
      break;
  }
}

ALWAYS_INLINE static syms::Scope* newScope(Arena* arena, syms::Scope* parentScope, syms::ScopeKind kind) {
  auto result = new (arena) syms::Scope(arena);
  result->init(parentScope, kind);

  return result;
}

ALWAYS_INLINE static Name* nameOf(ast::Node* node) {
#ifdef DEBUG
  if(node->kind() != ast::NodeKind::kIdentifier) {
    std::cerr << "Invalid node." << std::endl;
  }
#endif
  
  return static_cast<ast::Identifier*>(node)->name();
}

void SymbolsPhase::buildClassSymbols(ast::Class* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kClass);
  auto symbol =  new (m_arena) syms::ClassSymbol();
  auto type = new (m_arena) types::ClassType(symbol, scope, 0, nullptr, 0, nullptr);
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope, type);
  node->symbol(symbol);
  parentScope->put(name, symbol);

  node->members()->foreach([&](ast::Node* member) {
    buildSymbols(member, scope, symbol);
  });
}

void SymbolsPhase::buildFunctionSymbols(ast::Function* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kFunction);
  auto symbol =  new (m_arena) syms::FunctionSymbol();
  auto type = new (m_arena) types::FunctionType(
      symbol, scope, 0, nullptr, parentSymbol->type(),
      node->parameters()->size(),
      node->parameters()->mapToArray<syms::Symbol*>(
        [&](ast::Node* node) -> syms::Symbol* {
          switch(node->kind()) {
            case ast::NodeKind::kIdentifier:
              return static_cast<ast::Identifier*>(node)->symbol();
            default:
              return nullptr; //TODO(joa): fixme
          }
        }, m_arena)
      , nullptr);
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope, type);
  node->symbol(symbol);
  parentScope->putOrOverload(name, symbol);

  node->parameters()->foreach([&](ast::Node* parameter) {
    buildSymbols(parameter, scope, symbol);
  });

  if(!node->isAbstract()) {
    buildSymbols(node->expr(), scope, symbol);
  }
}

void SymbolsPhase::buildModuleSymbols(ast::Module* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kModule);
  auto symbol =  new (m_arena) syms::ModuleSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope, nullptr); //TODO(joa): module type?
  node->symbol(symbol);
  parentScope->put(name, symbol);

  node->declarations()->foreach([&](ast::Node* declaration) {
    buildSymbols(declaration, scope, symbol);
  });
}

void SymbolsPhase::buildVariableSymbol(ast::Variable* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto symbol =  new (m_arena) syms::VariableSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node);
  node->symbol(symbol);
  parentScope->put(name, symbol);
}

void SymbolsPhase::buildParameterSymbol(ast::Parameter* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto symbol =  new (m_arena) syms::VariableSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node);
  node->symbol(symbol);
  parentScope->put(name, symbol);
}

void SymbolsPhase::buildBlockScope(ast::Block* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kBlock);
  auto symbol = new (m_arena) syms::EmptySymbol();

  symbol->init(parentSymbol, node);
  node->scope(scope);
  node->symbol(symbol);

  node->expressions()->foreach([&](ast::Node* expression) {
    buildSymbols(expression, parentScope, symbol);
  });
}

void SymbolsPhase::buildProgramScope(ast::Program* node, syms::Scope* symbolTable) {
  node->modules()->foreach([&](ast::Node* module) {
    buildSymbols(module, symbolTable, /*parentSymbol=*/nullptr);
  });
}


void SymbolsPhase::buildIfSymbol(ast::If* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto symbol = new (m_arena) syms::EmptySymbol();
  
  symbol->init(parentSymbol, node);
  node->symbol(symbol);

  node->cases()->foreach([&](ast::Node* ifCase) {
    buildSymbols(ifCase, parentScope, symbol);
  });
}

void SymbolsPhase::buildIfCaseSymbol(ast::IfCase* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto symbol = new (m_arena) syms::EmptySymbol();

  symbol->init(parentSymbol, node);
  node->symbol(symbol);

  buildSymbols(node->expr(), parentScope, symbol);
}

//

LinkPhase::LinkPhase(syms::Scope* symbolTable, Arena* arena)
    : Phase(arena), 
      m_symbolTable(symbolTable) {}

const char* LinkPhase::name() {
  return "LinkPhase";
}

void LinkPhase::apply(CompilationUnit* unit) {
  link(unit->ast(), m_symbolTable, /*parentType=*/nullptr);
}

syms::Symbol* LinkPhase::errorSymbol(syms::Symbol* parent, ast::Node* node, syms::ErrorReason reason) {
  auto result = new (m_arena) syms::ErrorSymbol();
  
  result->init(nullptr, parent, node, reason);

  return result;
}

void LinkPhase::link(ast::Node* node, syms::Scope* parentScope, types::Type* parentType) {
#define K(x) ast::NodeKind::k##x

#ifdef DEBUG
  if(nullptr == node) {
    std::cerr << "Node is null." << std::endl;
    return;
  }
#endif

  switch(node->kind()) {
    case K(Argument): {
        auto argument = static_cast<ast::Argument*>(node);

        if(argument->hasName()) {
          //TODO(joa): link to parameter sym
        }

        link(argument->value(), parentScope, parentType);
        argument->symbol(argument->value()->symbol());
      }
      break;
    case K(Assign):
      break;
    case K(Block): {
        auto block = static_cast<ast::Block*>(node);
        auto scope = block->scope();

        block->expressions()->foreach([&](ast::Node* expression) {
          link(expression, scope, parentType);
        });

        // The block symbol (which is empty) has already been
        // assigned in the symbols phase.
      }
      break;
    case K(Call):
      break;
    case K(Class): {
        auto klass = static_cast<ast::Class*>(node);
        auto symbol = static_cast<syms::ClassSymbol*>(klass->symbol());
        auto scope = symbol->scope();
        auto type = symbol->type();

        //TODO(joa): resolve bases of class type already?

        klass->members()->foreach([&](ast::Node* member) {
          link(member, scope, type);
        });
      }
      break;
    case K(Error):
      break;
    case K(False):
      break;
    case K(Function): {
        auto function = static_cast<ast::Function*>(node);
        auto symbol = static_cast<syms::FunctionSymbol*>(function->symbol());
        auto scope = symbol->scope();
        auto type = symbol->type();

        if(!function->isAbstract()) {
          link(function->expr(), scope, type);
        }
      }
      break;
    case K(Identifier): {
        auto ident = static_cast<ast::Identifier*>(node);
        auto symbol = parentScope->get(ident->name());

        if(nullptr == symbol) {
          ident->symbol(
            errorSymbol(parentType->symbol(), ident, syms::ErrorReason::kNoSuchName));
        } else {
          ident->symbol(symbol);
        }
      }
      break;
    case K(If): {
        auto iff = static_cast<ast::If*>(node);
        iff->cases()->foreach([&](ast::Node* ifCase) {
          link(ifCase, parentScope, parentType);
        });
        //TODO(joa): lub? type?

        // EmptySymbol has already been built in 
        // the symbols phase.
      }
      break;
    case K(IfCase): {
        auto ifCase = static_cast<ast::IfCase*>(node);
        link(ifCase->condition(), parentScope, parentType);
        link(ifCase->expr(), parentScope, parentType);
        
        //TODO(joa): type?

        // EmptySymbol has already been built in 
        // the symbols phase.
      }
      break;
    case K(Module): {
        auto module = static_cast<ast::Module*>(node);
        auto symbol = static_cast<syms::ModuleSymbol*>(module->symbol());
        auto scope = symbol->scope();
        auto type = symbol->type();

        module->declarations()->foreach([&](ast::Node* declaration) {
          link(declaration, scope, type);
        });
      }
      break;
    case K(ModuleDependency):
      break;
    case K(Number):
      break;
    case K(Parameter): 
      // Parameter symbol has been built in SymbolsPhase
      break;
    case K(Program): {
        auto program = static_cast<ast::Program*>(node);
        program->modules()->foreach([&](ast::Node* module) {
          link(module, parentScope, parentType);
        });
      }
      break;
    case K(Select): {
        auto select = static_cast<ast::Select*>(node);
        link(select->object(), parentScope, parentType);
        link(select->qualifier(), parentScope, parentType);
        //TODO(joa): set sym of select
      }
      break;
    case K(String):
      break;
    case K(This): {
        auto thiz = static_cast<ast::This*>(node);
        auto type = parentType;
          
        while(type != nullptr && type->kind() != types::TypeKind::kClass) {
          if(type->kind() == types::TypeKind::kFunction) {
            auto functionType = static_cast<types::FunctionType*>(type);
            type = functionType->owner();
          } else {
            std::cerr << "Unknown type." << std::endl;
          }
        }
       
        if(nullptr == type) {
          //TODO(joa): error symbol
        } else {
          auto classType = static_cast<types::ClassType*>(type);
          thiz->symbol(classType->symbol());
        }
      }
      break;
    case K(True):
      break;
    case K(TypeParameter):
      break;
    case K(Variable):
      // Variable symbol has been built in SymbolsPhase
      break;
    default:
      std::cerr << "Internal error." << std::endl;
  }
}

} //namespace internal
} //namespace brutus