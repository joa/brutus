#include "symbols.h"

namespace brutus {
namespace internal {
namespace syms {
template<symbolKind::Value K>
DeclarativeSymbol<K>::DeclarativeSymbol()
  : m_declarations(nullptr) {}

template<symbolKind::Value K>
symbolKind::Value DeclarativeSymbol<K>::kind() const {
  return K;
}

template<symbolKind::Value K>
void DeclarativeSymbol<K>::init(Name* name, Symbol* parent, ast::Node* ast, Scope* scope) {
  Symbol::init(name, parent, ast);
  m_declarations = scope;
}

template<symbolKind::Value K>
Scope* DeclarativeSymbol<K>::declarations() const {
  return m_declarations;
}

//

ErrorSymbol::ErrorSymbol() 
    : m_reason(errorReason::kUnknown) {}

symbolKind::Value ErrorSymbol::kind() const {
  return symbolKind::kError;
}

void ErrorSymbol::init(Name* name, Symbol* parent, ast::Node* ast, errorReason::Value reason) {
  Symbol::init(name, parent, ast);
  m_reason = reason;
}

errorReason::Value ErrorSymbol::reason() const {
  return m_reason;
}

//

OverloadSymbol::OverloadSymbol() 
    : m_first(nullptr) {}

symbolKind::Value OverloadSymbol::kind() const {
  return symbolKind::kOverload;
}

void OverloadSymbol::init(Name* name, Symbol* parent, ast::Node* ast) {
  Symbol::init(name, parent, ast);
}

void OverloadSymbol::add(Symbol* symbol) {
  symbol->m_next = m_first;
  m_first = symbol;
}

//

VariableSymbol::VariableSymbol() {}

symbolKind::Value VariableSymbol::kind() const {
  return symbolKind::kVariable;
}

void VariableSymbol::init(Name* name, Symbol* parent, ast::Node* ast) {
  Symbol::init(name, parent, ast);
}
} //namespace syms
} //namespace internal
} //namespace brutus
