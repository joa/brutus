#include "symbols.h"

namespace brutus {
namespace internal {
namespace syms {
ErrorSymbol::ErrorSymbol() 
    : m_reason(ErrorReason::kUnknown) {}

SymbolKind ErrorSymbol::kind() const {
  return SymbolKind::kError;
}

void ErrorSymbol::init(Name* name, Symbol* parent, ast::Node* ast, ErrorReason reason) {
  Symbol::init(name, parent, ast, nullptr);//TODO(joa): ERROR TYPE
  m_reason = reason;
}

ErrorReason ErrorSymbol::reason() const {
  return m_reason;
}

//

OverloadSymbol::OverloadSymbol() 
    : m_first(nullptr) {}

SymbolKind OverloadSymbol::kind() const {
  return SymbolKind::kOverload;
}

void OverloadSymbol::init(Name* name, Symbol* parent, ast::Node* ast) {
  Symbol::init(name, parent, ast, nullptr);//TODO(joa): OVERLOAD TYPE

  auto symbol = m_first;

  while(symbol != nullptr) {
    symbol->init(name, parent, ast, symbol->type());
    symbol = symbol->m_next;
  }
}

void OverloadSymbol::add(Symbol* symbol) {
  symbol->m_next = m_first;
  m_first = symbol;
}

//

VariableSymbol::VariableSymbol() {}

SymbolKind VariableSymbol::kind() const {
  return SymbolKind::kVariable;
}

void VariableSymbol::init(Name* name, Symbol* parent, ast::Node* ast) {
  Symbol::init(name, parent, ast, nullptr);
}

//

EmptySymbol::EmptySymbol() {}

SymbolKind EmptySymbol::kind() const {
  return SymbolKind::kEmpty;
}

void EmptySymbol::init(Symbol* parent, ast::Node* ast) {
  Symbol::init(nullptr, parent, ast, nullptr);
}
} //namespace syms
} //namespace internal
} //namespace brutus
