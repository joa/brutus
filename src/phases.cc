#include "phases.h"

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

SymbolsPhase::SymbolsPhase(syms::Scope* symbolTable, Arena* arena)
    : Phase(arena), 
      m_symbolTable(symbolTable) {}

const char* SymbolsPhase::name() {
  return "SymbolsPhase";
}

void SymbolsPhase::apply(ast::Node* node) {
  buildSymbols(node, m_symbolTable, nullptr);
}

void SymbolsPhase::buildSymbols(ast::Node* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  switch(node->kind()) {
    case ast::NodeKind::kClass:
      buildClassSymbols(static_cast<ast::Class*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kFunction:
      buildFunctionSymbols(static_cast<ast::Function*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kModule:
      buildModuleSymbols(static_cast<ast::Module*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kVariable:
      buildVariableSymbol(static_cast<ast::Variable*>(node), parentScope, parentSymbol);
      break;

    case ast::NodeKind::kBlock:
      buildBlockScope(static_cast<ast::Block*>(node), parentScope, parentSymbol);
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
  assert(node->kind() == ast::NodeKind::kIdentifier);
#endif
  
  return static_cast<ast::Identifier*>(node)->name();
}
void SymbolsPhase::buildClassSymbols(ast::Class* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kClass);
  auto symbol =  new (m_arena) syms::ClassSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope);

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

  parentScope->put(name, symbol);

  buildSymbols(node->expr(), scope, symbol);
}

void SymbolsPhase::buildModuleSymbols(ast::Module* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kModule);
  auto symbol =  new (m_arena) syms::ModuleSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope);

  parentScope->put(name, symbol);

  node->declarations()->foreach([&](ast::Node* declaration) {
    buildSymbols(declaration, scope, symbol);
  });
}

void SymbolsPhase::buildVariableSymbol(ast::Variable* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto symbol =  new (m_arena) syms::VariableSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node);

  parentScope->put(name, symbol);
}

void SymbolsPhase::buildBlockScope(ast::Block* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_arena, parentScope, syms::ScopeKind::kBlock);

  node->expressions()->foreach([&](ast::Node* expression) {
    buildSymbols(expression, parentScope, parentSymbol);
  });
}
} //namespace internal
} //namespace brutus