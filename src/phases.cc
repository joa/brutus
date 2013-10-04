#include "phases.h"
#include "compiler.h"
#include "types.h"

namespace brutus {
namespace internal {
Phase::Phase(Context* context)
    : m_context(context) {}

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

ParsePhase::ParsePhase(Context* context)
    : Phase(context),
      m_lexer(new internal::Lexer()),
      m_parser(new internal::Parser(m_lexer, context->names(), context->arena())) {}

ParsePhase::~ParsePhase() {
  delete m_lexer;
  delete m_parser;
}

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
        std::cout << std::endl;

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

SymbolsPhase::SymbolsPhase(Context* context)
    : Phase(context) {}

const char* SymbolsPhase::name() {
  return "SymbolsPhase";
}

void SymbolsPhase::apply(CompilationUnit* unit) {
  buildSymbols(unit->ast(), m_context->symbols()->global(), nullptr);
}

void SymbolsPhase::buildSymbols(ast::Node* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
#define K(x) ast::NodeKind::k##x

#ifdef DEBUG
  if(nullptr == node) {
    std::cerr << "Node is null." << std::endl;
    return;
  }
#endif

  switch(node->kind()) {
    case K(Assign):
    case K(Argument): {
        auto symbol = new (m_context->arena()) syms::EmptySymbol();
        symbol->init(parentSymbol, node);
        node->symbol(symbol);
      }
      break;

    case K(Block):
      buildBlockScope(static_cast<ast::Block*>(node), parentScope, parentSymbol);
      break;

    case K(Class):
      buildClassSymbols(static_cast<ast::Class*>(node), parentScope, parentSymbol);
      break;

    case K(Function):
      buildFunctionSymbols(static_cast<ast::Function*>(node), parentScope, parentSymbol);
      break;

    case K(Module):
      buildModuleSymbols(static_cast<ast::Module*>(node), parentScope, parentSymbol);
      break;

    case K(Parameter):
      buildParameterSymbol(static_cast<ast::Parameter*>(node), parentScope, parentSymbol);
      break;

    case K(Program):
      buildProgramScope(static_cast<ast::Program*>(node), parentScope);
      break;

    case K(Variable):
      buildVariableSymbol(static_cast<ast::Variable*>(node), parentScope, parentSymbol);
      break;

    case K(If):
      buildIfSymbol(static_cast<ast::If*>(node), parentScope, parentSymbol);
      break;

    case K(IfCase):
      buildIfCaseSymbol(static_cast<ast::IfCase*>(node), parentScope, parentSymbol);
      break;

    case K(Call):
    case K(Error):
    case K(False):
    case K(Identifier):
    case K(ModuleDependency):
    case K(Number):
    case K(Select):
    case K(String):
    case K(This):
    case K(True):
    case K(TypeParameter):
      break;
  }

#undef K
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
  auto scope = newScope(m_context->arena(), parentScope, syms::ScopeKind::kClass);
  auto symbol = new (m_context->arena()) syms::ClassSymbol();
  auto type = new (m_context->arena()) types::ClassType(symbol, scope, 0, nullptr, 0, nullptr);
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope, type);
  node->symbol(symbol);
  parentScope->put(name, symbol);

  node->members()->foreach([&](ast::Node* member) {
    buildSymbols(member, scope, symbol);
  });
}

void SymbolsPhase::buildFunctionSymbols(ast::Function* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_context->arena(), parentScope, syms::ScopeKind::kFunction);
  auto symbol = new (m_context->arena()) syms::FunctionSymbol();
  auto type = new (m_context->arena()) types::FunctionType(
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
        }, m_context->arena())
      , nullptr);
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope, type);
  node->symbol(symbol);
  parentScope->putOrOverload(name, symbol);

  node->parameters()->foreach([&](ast::Node* parameter) {
    buildSymbols(parameter, scope, symbol);
  });

  // The parameter symbols are known, patch the type.

  type->m_parameters =
    node->parameters()->mapToArray<syms::Symbol*>(
          [&](ast::Node* node) -> syms::Symbol* {
            return node->symbol();
          }, m_arena);

  if(!node->isAbstract()) {
    buildSymbols(node->expr(), scope, symbol);
  }
}

void SymbolsPhase::buildModuleSymbols(ast::Module* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_context->arena(), parentScope, syms::ScopeKind::kModule);
  auto symbol =  new (m_context->arena()) syms::ModuleSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node, scope, nullptr); //TODO(joa): module type?
  node->symbol(symbol);
  parentScope->put(name, symbol);

  node->declarations()->foreach([&](ast::Node* declaration) {
    buildSymbols(declaration, scope, symbol);
  });
}

void SymbolsPhase::buildVariableSymbol(ast::Variable* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto symbol =  new (m_context->arena()) syms::VariableSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node);
  node->symbol(symbol);
  parentScope->put(name, symbol);
}

void SymbolsPhase::buildParameterSymbol(ast::Parameter* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto symbol =  new (m_context->arena()) syms::VariableSymbol();
  auto name = nameOf(node->name());

  symbol->init(name, parentSymbol, node);
  node->symbol(symbol);
  parentScope->put(name, symbol);
}

void SymbolsPhase::buildBlockScope(ast::Block* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto scope = newScope(m_context->arena(), parentScope, syms::ScopeKind::kBlock);
  auto symbol = new (m_context->arena()) syms::EmptySymbol();

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
  auto symbol = new (m_context->arena()) syms::EmptySymbol();

  symbol->init(parentSymbol, node);
  node->symbol(symbol);

  node->cases()->foreach([&](ast::Node* ifCase) {
    buildSymbols(ifCase, parentScope, symbol);
  });
}

void SymbolsPhase::buildIfCaseSymbol(ast::IfCase* node, syms::Scope* parentScope, syms::Symbol* parentSymbol) {
  auto symbol = new (m_context->arena()) syms::EmptySymbol();

  symbol->init(parentSymbol, node);
  node->symbol(symbol);

  buildSymbols(node->expr(), parentScope, symbol);
}

//

LinkPhase::LinkPhase(Context* context)
    : Phase(context) {}

const char* LinkPhase::name() {
  return "LinkPhase";
}

void LinkPhase::apply(CompilationUnit* unit) {
  link(unit->ast(), m_context->symbols()->global(), /*parentType=*/nullptr);
}

syms::Symbol* LinkPhase::errorSymbol(syms::Symbol* parent, ast::Node* node, syms::ErrorReason reason) {
  auto result = new (m_context->arena()) syms::ErrorSymbol();

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
        link(argument->value(), parentScope, parentType);

        // The empty symbol has already been assigned. If
        // this argument has a name it is linked to its
        // parameter. However the linkage happens in the
        // code that solves a call.
      }
      break;
    case K(Assign): {
        auto assign = static_cast<ast::Assign*>(node);
        link(assign->target(), parentScope, parentType);
        link(assign->value(), parentScope, parentType);
        // The empty symbol has already been assigned.
      }
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
    case K(Call): {
        auto call = static_cast<ast::Call*>(node);

        link(call->callee(), parentScope, parentType);

        auto calleeSymbol = call->callee()->symbol();
        auto calleeType = calleeSymbol->type();
        auto calleeScope = calleeType->scope();

        call->arguments()->foreach([&](ast::Node* argument) {
          link(argument, parentScope, parentType);

          if(argument->kind() == ast::NodeKind::kArgument) {
            ast::Argument* arg = static_cast<ast::Argument*>(argument);

            if(arg->hasName()) {
              auto parameterSymbol = calleeScope->get(nameOf(arg->name()));

              if(nullptr == parameterSymbol) {
                arg->symbol(
                  errorSymbol(parentType->symbol(), arg, syms::ErrorReason::kNoSuchName));
              } else {
                arg->symbol(parameterSymbol);
              }
            }
          }
        });

        call->symbol(call->callee()->symbol());
      }
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
      //TODO(joa): Link to Boolean Symbol
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
        // Perform the following steps. If one succeeds it is no
        // no longer necessary to perform the consecutive actions.
        //
        //   1) Try to find ident in current scope (includes members of class up to module)
        //   2) Try to find ident in scope of any base class
        //   3) Try to find ident in any module dependency
        //   4) Result in error
        //

        auto ident = static_cast<ast::Identifier*>(node);
        auto symbol = parentScope->get(ident->name());

        if(nullptr == symbol) {
          // Could not find name in current scope. Maybe we
          // should try to find it

          //TODO(joa): implement me
        }

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

        // EmptySymbol has already been built in
        // the symbols phase.
      }
      break;
    case K(IfCase): {
        auto ifCase = static_cast<ast::IfCase*>(node);
        link(ifCase->condition(), parentScope, parentType);
        link(ifCase->expr(), parentScope, parentType);

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
    case K(Number): {
        auto number = static_cast<ast::Number*>(node);
        auto symbol = m_context->symbols()->get(
          m_context->names()->brutus_Int());

        if(nullptr == symbol) {
          // Only possible if predef is missing
          number->symbol(
            errorSymbol(nullptr, number, syms::ErrorReason::kNoSuchName));
        } else {
          number->symbol(symbol);
        }
      }
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

        auto objectSymbol = select->object()->symbol();
        auto objectType = objectSymbol->type();
        auto objectScope = objectType->scope();
        auto symbol = objectScope->get(nameOf(select->qualifier()));

        if(nullptr == symbol) {
          select->symbol(
            errorSymbol(nullptr, select, syms::ErrorReason::kNoSuchName));
        } else {
          select->symbol(symbol);
        }
      }
      break;
    case K(String): {
        auto number = static_cast<ast::String*>(node);
        auto symbol = m_context->symbols()->get(
          m_context->names()->brutus_String());

        if(nullptr == symbol) {
          // Only possible if predef is missing
          number->symbol(
            errorSymbol(nullptr, number, syms::ErrorReason::kNoSuchName));
        } else {
          number->symbol(symbol);
        }
      }
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
          thiz->symbol(
            errorSymbol(nullptr, thiz, syms::ErrorReason::kNoSuchName));
        } else {
          auto classType = static_cast<types::ClassType*>(type);
          thiz->symbol(classType->symbol());
        }
      }
      break;
    case K(True):
      //TODO(joa): Link to Boolean Symbol
      break;
    case K(TypeParameter):
      break;
    case K(Variable):
      // Variable symbol has been built in SymbolsPhase
      break;
    default:
      std::cerr << "Internal error." << std::endl;
  }

#undef K
}

} //namespace internal
} //namespace brutus
