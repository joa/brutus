#include "phases.h"
#include "compiler.h"

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

        //auto printer = new brutus::internal::ast::ASTPrinter(std::cout);     
        //printer->print(unit->ast());

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

    case ast::NodeKind::kProgram:
      buildProgramScope(static_cast<ast::Program*>(node), parentScope);
      break;

    case ast::NodeKind::kVariable:
      buildVariableSymbol(static_cast<ast::Variable*>(node), parentScope, parentSymbol);
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
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope);
  node->symbol(symbol);
  parentScope->put(name, symbol);

  node->members()->foreach([&](ast::Node* member) {
    buildSymbols(member, scope, symbol);
  });
}

void SymbolsPhase::buildFunctionSymbols(ast::Function* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kFunction);
  auto symbol =  new (m_arena) syms::FunctionSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope);
  node->symbol(symbol);
  parentScope->putOrOverload(name, symbol);

  if(!node->isAbstract()) {
    buildSymbols(node->expr(), scope, symbol);
  }
}

void SymbolsPhase::buildModuleSymbols(ast::Module* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kModule);
  auto symbol =  new (m_arena) syms::ModuleSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope);
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

void SymbolsPhase::buildBlockScope(ast::Block* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kBlock);

  node->scope(scope);

  node->expressions()->foreach([&](ast::Node* expression) {
    buildSymbols(expression, parentScope, parentSymbol);
  });
}

void SymbolsPhase::buildProgramScope(ast::Program* node, syms::Scope* symbolTable) {
  node->modules()->foreach([&](ast::Node* module) {
    buildSymbols(module, symbolTable, /*parentSymbol=*/nullptr);
  });
}

//

LinkPhase::LinkPhase(syms::Scope* symbolTable, Arena* arena)
    : Phase(arena), 
      m_symbolTable(symbolTable) {}

const char* LinkPhase::name() {
  return "LinkPhase";
}

void LinkPhase::apply(CompilationUnit* unit) {
  link(unit->ast(), m_symbolTable);
}

void LinkPhase::link(ast::Node* node, syms::Scope* scope) {
#define K(x) ast::NodeKind::k##x

#ifdef DEBUG
  if(nullptr == node) {
    std::cerr << "Node is null." << std::endl;
    return;
  }
#endif

  switch(node->kind()) {
    case K(Argument):
    case K(Assign):
    case K(Block):
    case K(Call):
    case K(Class):
    case K(Error):
    case K(False):
    case K(Function):
    case K(Identifier):
    case K(If):
    case K(IfCase):
    case K(Module):
    case K(ModuleDependency):
    case K(Number):
    case K(Parameter):
    case K(Program):
    case K(Select):
    case K(String):
    case K(This):
    case K(True):
    case K(TypeParameter):
    case K(Variable):
      break;
  }
}

} //namespace internal
} //namespace brutus