#include "parser.h"

namespace brutus {
namespace internal {
//
// Comments define the Brutus grammar in something similar to
// EBNF. The following conventions are used:
//
//   *     = 0 to n repitions
//   +     = 1 to n repititions
//   |     = disjunction
//   ()    = Sequence
//   ?     = Optional
//   UPPER = Token
//   Lower = Production rule
//   '{'   = Shorthand for token LBRACE for instance
//
// Example:
//
// Block
//  : '{' Expression (NEWLINE Expression)* '}'
//  | Expression
//
// The Block production rule is read as following:
//
// There are two cases. The first is LBRACE followed by the Expression rule.
// Then there are 0 ore more repitions of a newline token followed by an
// Expression. This is followed by a final RBRACE token.
//
// The second case is just a single Expression.
//

// Macro to expect a certain token type. If no such token is found
// the ast::Error token is returned.

#ifdef _MSC_VER
#define SNPRINTF _snprintf
#else
#define SNPRINTF snprintf
#endif

#define EXPECT(t) \
  if(!poll(t)) { \
    auto buf = m_arena->newArray<char>(0x100); \
    SNPRINTF(buf, 0x100, "%s:%d Invalid syntax. Expected %s, got %s.", __FILE__, \
      __LINE__, tok::toString(t), tok::toString(m_currentToken)); \
    return error(buf); \
  }

//
// Program
//  : (Module NEWLINE)*
//
ast::Node* Parser::parseProgram() {
  advance();

  auto result = alloc<ast::Program>();
  auto modules = result->modules();

  while(peek(Token::kModule)) {
    modules->add(parseModule(), m_arena);
    EXPECT(Token::kNewLine);
  }

  return result;
}


//
// Module
//  : 'module' ActorName '{' NEWLINE (ModuleDeclaration NEWLINE)* '}'
//  ;
//

//
// ModuleDeclaration
//  : ModuleDependency
//  | Declaration
//  | Expression
//  ;
//
ast::Node* Parser::parseModule() {
  EXPECT(Token::kModule);

  auto result = alloc<ast::Module>();
  ast::Node* name = nullptr;

  if(peek(Token::kIdentifier)) {
     name = parseIdentifier(); //TODO(joa): parseActorName()
  } else {
    auto emptyName = alloc<ast::Identifier>();
    emptyName->init(m_names->empty());

    name = emptyName;
  }

  result->init(name);

  EXPECT(Token::kLBrace);
  EXPECT(Token::kNewLine);

  auto dependencies = result->dependencies();
  auto declarations = result->declarations();

  do {
    // ModuleDeclaration
    if(peek(Token::kRequire)) {
      dependencies->add(parseModuleDependency(), m_arena);
    } else {
      ast::Node* declaration = parseDeclaration();

      if(nullptr == declaration) {
        declarations->add(parseExpression(), m_arena);
      } else {
        declarations->add(declaration, m_arena);
      }
    }
    EXPECT(Token::kNewLine);
  } while(!peek(Token::kRBrace));

  EXPECT(Token::kRBrace);

  return result;
}

//
// ModuleDependency
//  : 'require' ActorName ModuleVersion?
//  ;

ast::Node* Parser::parseModuleDependency() {
  EXPECT(Token::kRequire);

  auto result = alloc<ast::ModuleDependency>();
  auto name = parseIdentifier(); //TODO(joa): parseActorName()

  //TODO(joa):
  result->init(name, peek(Token::kColon) ? parseModuleVersion() : nullptr);

  return result;
}

//
// ModuleVersion
//  : ':' ModuleVersionRange
//

//
// ModuleVersionRange
//  : ModuleVersionRange (',' ModuleVersionRange)*
//  | NumberLiteral
//  | '[' NumberLiteral ']'
//  | ('[' | '(') NumberLiteral ',' NumberLiteral? (']' | ')')
//  | ('[' | '(') NumberLiteral? ',' NumberLiteral (']' | ')')
//

ast::Node* Parser::parseModuleVersion() {
  EXPECT(Token::kColon);

  bool isBracket;

  if(poll(Token::kLBrac)) {
    isBracket = YES;
  } else if(poll(Token::kLParen)) {
    isBracket = NO;
  } else {
    auto number = parseNumberLiteral();

    //TODO(joa): NumberLiteral
    UNUSED(number);

    return error("TODO NumberLiteral");
  }

  if(poll(Token::kComma)) {
    // ?,
    auto x = parseNumberLiteral();

    UNUSED(x);

    if(poll(Token::kRBrac)) {
      //?,x]
      return error("TODO ?,x]");
    } else if(poll(Token::kRParen)) {
      //?,x)
      return error("TODO ?,x)");
    } else {
      return error("Expected ']' or ')'");
    }
  } else {
    // ?x
    auto x = parseNumberLiteral();

    UNUSED(x);

    if(poll(Token::kComma)) {
      //[x,
      if(poll(Token::kRBrac)) {
        // [x,]
        return error("TODO ?x,]");
      } else if(poll(Token::kRParen)) {
        // [x,)
        return error("TODO ?x,)");
      } else {
        //[x,y
        auto y = parseNumberLiteral();

        UNUSED(y);

        if(poll(Token::kRBrac)) {
          //[x,y]
          return error("TODO ?x,y]");
        } else if(poll(Token::kRParen)) {
          //[x,y)
          return error("TODO ?x,y)");
        } else {
          return error("Expected ']' or ')'");
        }
      }
    } else if(isBracket && poll(Token::kRBrac)) {
      //[x]
      //return
      return error("TODO [x]");
    } else {
      return error("Expected ',' or ']'");
    }
  }
}

//
// Block
//  : '{' NEWLINE (Block NEWLINE)+ '}'
//  | Expression
//  | Declaration
//  ;
//
ast::Node* Parser::parseBlock() {
  if(poll(Token::kLBrace)) {
    EXPECT(Token::kNewLine);
    auto block = alloc<ast::Block>();
    auto expressions = block->expressions();

    do {
      expressions->add(parseBlock(), m_arena);
      EXPECT(Token::kNewLine);
    } while(!peek(Token::kRBrace));

    EXPECT(Token::kRBrace);
    return block;
  } else {
    ast::Node* declaration = parseDeclaration();

    if(nullptr == declaration) {
      return parseExpression();
    }

    return declaration;
  }
}


//
// Declaration
//  : Function
//  | Class
//  | Variable
//  ;
//
ast::Node* Parser::parseDeclaration() {
  pollAll(Token::kNewLine);

  if(peek(Token::kDef)) {
    return parseFunction(Parser::ACC_PUBLIC);
  } else if(peek(Token::kClass)) {
    return parseClass(Parser::ACC_PUBLIC);
  } else if(peek(Token::kVal) || peek(Token::kVar)) {
    return parseVariable(Parser::ACC_PUBLIC);
  } else if(peekVisibility()) {
    unsigned int flags = 0;

    if(poll(Token::kPublic)) {
      flags |= Parser::ACC_PUBLIC;
    } else if(poll(Token::kPrivate)) {
      flags |= Parser::ACC_PRIVATE;
    } else if(poll(Token::kProtected)) {
      flags |= Parser::ACC_PROTECTED;
    } else if(poll(Token::kInternal)) {
      flags |= Parser::ACC_INTERNAL;
    }

    if(poll(Token::kImmutable)) {
      flags |= Parser::ACC_IMMUTABLE;
    }

    if(poll(Token::kPure)) {
      flags |= Parser::ACC_PURE;
    }

    if(poll(Token::kVirtual)) {
      flags |= Parser::ACC_VIRTUAL;
    }

    if(poll(Token::kNative)) {
      flags |= Parser::ACC_NATIVE;
    }

    if(peek(Token::kDef)) {
      if((flags & Parser::ACC_IMMUTABLE) != 0) {
        return error("Function may not be marked immutable.");
      }

      return parseFunction(flags);
    } else if(peek(Token::kClass)) {
      if((flags & Parser::ACC_PURE) != 0) {
        return error("Class may not be marked pure.");
      }

      return parseClass(flags);
    } else if(peek(Token::kVal) || peek(Token::kVar)) {
      if((flags & Parser::ACC_IMMUTABLE) != 0) {
        return error("Variable may not be marked immutable.");
      }

      if((flags & Parser::ACC_PURE) != 0) {
        return error("Variable may not be marked pure.");
      }

      return parseVariable(flags);
    } else {
      return error("Expected 'def', 'var', 'val' or 'class'.");
    }
  } else if(poll(Token::kImmutable)) {
    unsigned int flags = Parser::ACC_PUBLIC | Parser::ACC_IMMUTABLE;

    if(poll(Token::kVirtual)) {
      flags |= Parser::ACC_VIRTUAL;
    }

    if(poll(Token::kNative)) {
      flags |= Parser::ACC_NATIVE;
    }

    if(peek(Token::kClass)) {
      return parseClass(flags);
    } else {
      return error("Expected 'class'.");
    }
  } else if(poll(Token::kPure)) {
    unsigned int flags = Parser::ACC_PUBLIC | Parser::ACC_PURE;

    if(poll(Token::kVirtual)) {
      flags |= Parser::ACC_VIRTUAL;
    }

    if(poll(Token::kNative)) {
      flags |= Parser::ACC_NATIVE;
    }

    if(peek(Token::kDef)) {
      return parseFunction(flags);
    } else {
      return error("Expected 'def'.");
    }
  } else if(poll(Token::kVirtual)) {
    // "virtual class" is the same as "public virtual class"
    unsigned int flags = Parser::ACC_PUBLIC | Parser::ACC_VIRTUAL;

    if(poll(Token::kNative)) {
      flags |= Parser::ACC_NATIVE;
    }

    if(peek(Token::kDef)) {
      return parseFunction(flags);
    } else if(peek(Token::kClass)) {
      return parseClass(flags);
    } else if(peek(Token::kVal) || peek(Token::kVar)) {
      return parseVariable(flags);
    } else {
      return error("Expected 'def', 'val', 'var' or 'class'.");
    }
  } else {
    return nullptr;
  }
}

bool Parser::peekVisibility() {
  return peek(Token::kPublic)
      || peek(Token::kPrivate)
      || peek(Token::kProtected)
      || peek(Token::kInternal);
}

//
// Visibility
//  : Public
//  | Private
//  | Protected
//  | Internal
//  ;

//
// Class
//  : Visibility? 'virtual'? 'native'? 'class' Identifier TypeParameter?
//      Constructor Extends? '{' NEWLINE (Definition NEWLINE)* '}'
//
ast::Node* Parser::parseClass(unsigned int flags) {
  if(0 == flags) {
    return error("Internal error.");
  }

  EXPECT(Token::kClass);

  auto name = parseIdentifier();
  auto result = alloc<ast::Class>();

  if(peek(Token::kLBrac)) {
    auto error = parseTypeParameterList(result->typeParameters());

    if(nullptr != error) {
      return error;
    }
  }

  if(poll(Token::kLParen)) {
    //TODO(joa): implement ctor
    EXPECT(Token::kRParen);
  }


  if(poll(Token::kColon)) { //TODO(joa): extends not colon
    //...
  }

  if(poll(Token::kLBrace)) {
    EXPECT(Token::kNewLine);

    auto members = result->members();

    while(!poll(Token::kRBrace)) {
      pollAll(Token::kNewLine);
      members->add(parseDeclaration(), m_arena);
      EXPECT(Token::kNewLine);
    }
  }

  result->init(name, flags);

  return result;
}

//
// ActorPath
//  : 'deploy' Type ('@' Path)?
//  | Type '@' Path
//

//
// Path
//   : '.' ('/' Path)?
//  | '..' ('/' Path)?
//  | '/' Path
//  | Identifier ('/' Path)?
//

// this is inconsistent since '{' Block '}' is not true. '{' and '}' are
// already part of the block but they must be present!
//
// Definition
//  : 'def' Identifier TypeParameterList? '(' ParameterList ')' Type '=' Block
//  | 'def' Identifier TypeParameterList? '(' ParameterList ')' '{' Block '}'
//
ast::Node* Parser::parseFunction(unsigned int flags) {
  EXPECT(Token::kDef);

  auto name = parseIdentifier();
  auto result = alloc<ast::Function>();

  if(peek(Token::kLBrac)) {
    auto error = parseTypeParameterList(result->typeParameters());

    if(nullptr != error) {
      return error;
    }
  }

  EXPECT(Token::kLParen);

  if(!peek(Token::kRParen)) {
    if(!parseParameterList(result->parameters())) {
      return error("Function exceeds max arity.");
    }
  }

  EXPECT(Token::kRParen);

  ast::Node* block = nullptr;
  ast::Node* type = nullptr;

  if(poll(Token::kColon)) {
    type = parseType();

    if(poll(Token::kAssign)) {
      block = parseBlock();
    } else if(peek(Token::kNewLine)) {
      flags |= ACC_ABSTRACT;
    } else {
      return error("Expected '=' or '\n'.");
    }
  } else if(peek(Token::kLBrace)) {
    block = parseBlock();
  } else if(peek(Token::kNewLine)) {
    flags |= ACC_ABSTRACT;
  } else {
    return error("Expected ':', '{' or '\n'.");
  }

  if((flags & ACC_ABSTRACT) != 0 && block != nullptr) {
    return error("Internal error.");
  }

  result->init(name, type, block, flags);

  return result;
}

bool Parser::parseParameterList(ast::NodeList* list) {
  int arity = 0;

  do {
    list->add(parseParameter(), m_arena);
    ++arity;

    if(arity > consts::MaxFunctionArity) {
      return false;
    }
  } while(poll(Token::kComma));

  return true;
}

ast::Node* Parser::parseParameter() {
  auto name = parseIdentifier();
  EXPECT(Token::kColon);
  auto type = parseType();
  auto result = alloc<ast::Parameter>();

  result->init(name, type);

  return result;
}

//
// A special case for the expression grammar: currently I am lazy and
// the fact that infix calls are left-associative is only expressed
// via a boolean that makes the production less greedy. So
// the option Expression Call* is always valid but it accepts only calls
// in the form of "foo.bar(baz)" if allowInfixCall is false. Otherwise
// it accepts "foo.bar(baz)" and "foo bar baz".
//
// Expression
//  : NEWLINE* '(' Expression ')'
//  | NEWLINE* Tuple
//  | NEWLINE* IfExpression
//  | NEWLINE* Expression Select*
//  | NEWLINE* Expression Call*
//  | NEWLINE* PrimaryExpression
//
ast::Node* Parser::parseExpression() {
  return parseExpression(true);
}

ast::Node* Parser::parseExpression(bool allowInfixCall) {
  pollAll(Token::kNewLine);

  ast::Node* expression = nullptr;

  if(poll(Token::kLParen)) {
    expression = parseExpression();

    if(peek(Token::kComma)) {
      int arity = 1;

      do {
        EXPECT(Token::kComma);

        parseExpression();

        ++arity;

        if(arity > consts::MaxTupleArity) {
          return error("Tuple exceeds max arity.");
        }
      } while(!poll(Token::kRParen));
    } else {
      EXPECT(Token::kRParen);
    }
  } else if(peek(Token::kLBrace)) {
    expression = parseBlock();
  } else if(peek(Token::kIf)) {
    expression = parseIfExpression();
  } else if(peekPrimaryExpression()) {
    expression = parsePrimaryExpression();
  }

  return continueWithExpression(expression, allowInfixCall);
}

//
// PrimaryExpression
//  : Identifier
//  | NumberLiteral
//  | BooleanLiteral
//  | StringLiteral
//  | This
//
ast::Node* Parser::parsePrimaryExpression() {
  if(peek(Token::kIdentifier)) {
    return parseIdentifier();
  } else if(peek(Token::kNumberLiteral)) {
    return parseNumberLiteral();
  } else if(peek(Token::kTrue) || peek(Token::kYes) ||
            peek(Token::kFalse) || peek(Token::kNo)) {
    return parseBooleanLiteral();
  } else if(peek(Token::kStringLiteral)) {
    return parseStringLiteral();
  } else if(peek(Token::kThis)) {
    return parseThis();
  } else {
    return error("Expected primary expression.");
  }
}

bool Parser::peekPrimaryExpression() {
  return peek(Token::kIdentifier)
      || peek(Token::kNumberLiteral)
      || peek(Token::kTrue)
      || peek(Token::kYes)
      || peek(Token::kFalse)
      || peek(Token::kNo)
      || peek(Token::kStringLiteral)
      || peek(Token::kThis);
}

ast::Node* Parser::continueWithExpression(
    ast::Node* expression, bool allowInfixCall) {
  if(nullptr == expression) {
    return error("Expected expression.");
  } else {
    while(true) {
      if(peek(Token::kDot)) {
        auto select = parseSelect(expression);
        expression = select;
      } if(peek(Token::kHash)) {
        //TODO(joa): implement ref
        //auto ref = parseRef(expression);
        //expression = ref;
        expression = error("TODO: implement #-select");
      } else if(peek(Token::kLParen) ||
          (allowInfixCall && peek(Token::kIdentifier))) {
        auto call = parseCall(expression);
        expression = call;
      } else if(peek(Token::kAssign)) {
        auto assign = parseAssign(expression);
        expression = assign;
      } else {
        break;
      }
    }

    return expression;
  }
}

//
// Select
//  : Expression '.' Identifier
//
ast::Node* Parser::parseSelect(ast::Node* object) {
  EXPECT(Token::kDot);

  auto qualifier = parseIdentifier();
  auto result = alloc<ast::Select>();

  result->init(object, qualifier);

  return result;
}

//
// Assign
//  : Expression '=' 'force'? Expression
//
//
ast::Node* Parser::parseAssign(ast::Node* target) {
  EXPECT(Token::kAssign);

  auto result = alloc<ast::Assign>();
  auto isForced = poll(Token::kForce);

  result->init(target, parseExpression(), isForced);

  return result;
}

//
// Call
//  : Expression '(' ArgumentList ')'
//  | Expression Identifier SingleArgument
//
ast::Node* Parser::parseCall(ast::Node* callee) {
  auto result = alloc<ast::Call>();

  if(poll(Token::kLParen)) {
    if(!peek(Token::kRParen)) {
      parseArgumentList(result->arguments());
    }

    EXPECT(Token::kRParen);

    result->init(callee);
  } else if(peek(Token::kIdentifier)) {
    auto name = parseIdentifier();
    auto select = alloc<ast::Select>();

    select->init(callee, name);
    result->init(select);

    result->arguments()->add(parseSingleArgument(), m_arena);
  } else {
    return error("Expected call.");
  }

  return result;
}

//
// ArgumentList
//  : Argument (',' Argument)*
//
void Parser::parseArgumentList(ast::NodeList* list) {
  do {
    list->add(parseArgument(), m_arena);
  } while(poll(Token::kComma));
}

//
// Argument
//  : Expression
//  | Identifier '=' Expression
//  | AnonymousFunctionExpression
//  | Identifier '=' AnonymousFunctionExpressione
//
ast::Node* Parser::parseArgument() {
  auto result = alloc<ast::Argument>();

  if(peek(Token::kIdentifier)) {
    auto ident = parseIdentifier();

    if(poll(Token::kAssign)) {
      if(peek(Token::kLBrace)) {
        result->init(ident, parseAnonymousFunctionExpression());
      } else {
        result->init(ident, parseExpression());
      }
    } else {
      result->init(nullptr, continueWithExpression(ident, true));
    }
  } else {
    if(peek(Token::kLBrace)) {
      result->init(nullptr, parseAnonymousFunctionExpression());
    } else {
      result->init(nullptr, parseExpression());
    }
  }

  return result;
}

//
// SingleArgument
//  : Expression
//  | AnonymousFunctionExpression
//
ast::Node* Parser::parseSingleArgument() {
  auto result = alloc<ast::Argument>();

  if(peek(Token::kLBrace)) {
    result->init(nullptr, parseAnonymousFunctionExpression());
  } else {
    result->init(nullptr, parseExpression(false));
  }

  return result;
}

//
// AnonymousFunctionExpression
//  : '{' NEWLINE* AnonymousFunctionParameterList
//    (':' Type?) ->' Block NEWLINE* '}'
//
ast::Node* Parser::parseAnonymousFunctionExpression() {
  EXPECT(Token::kLBrace);
  pollAll(Token::kNewLine);
  auto result = alloc<ast::Function>();
  parseAnonymousFunctionParameterList(result->parameters());
  ast::Node* returnType = nullptr;
  if(poll(Token::kColon)) {
    returnType = parseType();
  }
  EXPECT(Token::kRArrow);
  result->init(nullptr, returnType, parseBlock(), ACC_PUBLIC);
  pollAll(Token::kNewLine);
  EXPECT(Token::kRBrace);
  return result;
}

//
// AnonymousFunctionParameterList
//  : AnonymousFunctionParameter (',' AnonymousFunctionParameter)*
//
void Parser::parseAnonymousFunctionParameterList(ast::NodeList* parameters) {
  do {
    parameters->add(parseAnonymousFunctionParameter(), m_arena);
  } while(poll(Token::kComma));
}

//
// AnonymousFunctionParameter
//  : Identifier (':' Type)?
//
ast::Node* Parser::parseAnonymousFunctionParameter() {
  auto ident = parseIdentifier();
  auto type = poll(Token::kColon) ? parseType() : nullptr;
  auto result = alloc<ast::Parameter>();
  result->init(ident, type);
  return result;
}

//
// This
//  : 'this'
//
ast::Node* Parser::parseThis() {
  if(poll(Token::kThis)) {
    return alloc<ast::This>();
  }

  return error("Expected this.");
}

//
// BooleanLiteral
//  : 'yes'
//  | 'no'
//  | 'true'
//  | 'false'
//
ast::Node* Parser::parseBooleanLiteral() {
  if(poll(Token::kTrue) || poll(Token::kYes)) {
    return alloc<ast::True>();
  } else if(poll(Token::kFalse) || poll(Token::kNo)) {
    return alloc<ast::False>();
  } else {
    return error("Expected boolean literal.");
  }
}

//
// NumberLiteral
//  : NUMBER_LITERAL
//
ast::Node* Parser::parseNumberLiteral() {
  return consume(Token::kNumberLiteral, [&]() {
    return allocWithValue<ast::Number>();
  });
}

//
// StringLiteral
//  : STRING_LITERAL
//
ast::Node* Parser::parseStringLiteral() {
  return consume(Token::kStringLiteral, [&]() {
    return allocWithValue<ast::String>();
  });
}

//
// IfExpression
//  : if '{' NEWLINE (IfCase NEWLINE)+ '}'
//  | if IfCase
ast::Node* Parser::parseIfExpression() {
  auto result = alloc<ast::If>();

  EXPECT(Token::kIf);
  if(poll(Token::kLBrace)) {
    EXPECT(Token::kNewLine);
    auto cases = result->cases();
    do {
      cases->add(parseIfCase(), m_arena);
      EXPECT(Token::kNewLine);
    } while(!peek(Token::kRBrace));
    EXPECT(Token::kRBrace);
  } else {
    result->cases()->add(parseIfCase(), m_arena);
  }

  return result;
}

//
// IfCase
//  : Expression '->' Expression
//
ast::Node* Parser::parseIfCase() {
  auto condition = parseExpression();
  EXPECT(Token::kRArrow);
  auto block = parseExpression();
  auto result = alloc<ast::IfCase>();
  result->init(condition, block);
  return result;
}

//
// Variable
//  : ('val' | 'var') Identifier (((':' Type)? '=' Expression) | ':' Type)
//
ast::Node* Parser::parseVariable(unsigned int flags) {
  UNUSED(flags);

  bool modifiable = NO;
  bool isForced = NO;

  if(poll(Token::kVar)) {
    modifiable = YES;
  } else if(poll(Token::kVal)) {
    modifiable = NO;
  } else {
    return error("Expected \"var\" or \"val\".");
  }

  auto name = parseIdentifier();
  ast::Node* type = nullptr;

  if(poll(Token::kColon)) {
    // Optional type is present.
    type = parseType();
  }

  ast::Node* init = nullptr;

  if(poll(Token::kAssign)) {
    isForced = poll(Token::kForce);
    init = parseExpression();
  }

  if(nullptr == init && nullptr == type) {
    return error("Either a type or initializer must be given.");
  }

  auto result = alloc<ast::Variable>();

  result->init(modifiable, name, type, init, isForced);

  return result;
}

//
// Type
//  : Identifier ('[' Type ']')?
//  | '-> Type
//  | Type '->' Type
//  | '(' Type (',' Type)+ ')'
//
ast::Node* Parser::parseType() {
  auto mustBeImmutable = poll(Token::kImmutable);

  UNUSED(mustBeImmutable);

  //TODO(joa): wrap in type, add attrib
  if(poll(Token::kLParen)) {
    int arity = 1;

    parseType();

    do {
      EXPECT(Token::kComma);
      parseType();
      ++arity;

      if(arity > consts::MaxTupleArity) {
        return error("Tuple exceeds max arity.");
      }
    } while(!poll(Token::kRParen));

    return nullptr;
  } else if(poll(Token::kRArrow)) {
    parseType();
    return nullptr;
  } else {
    auto name = parseIdentifier();

    if(poll(Token::kLBrac)) {
      do {
        parseType();
      } while(poll(Token::kComma));
      EXPECT(Token::kRBrace);
    }

    if(poll(Token::kRArrow)) {
      parseType();
    }

    return name;
  }
}

//
// TypeParameterList
//  : '[' TypeParameter (',' TypeParameter)* ']'
//
ast::Node* Parser::parseTypeParameterList(ast::NodeList* list) {
  EXPECT(Token::kLBrac);

  do {
    list->add(parseTypeParameter(), m_arena);
  } while(poll(Token::kComma));

  EXPECT(Token::kRBrac);

  return nullptr;
}

//
// TypeParameter
//  : Identifier
//  | Identifier '->' Type
//  | Identifier '<-' Type
//
ast::Node* Parser::parseTypeParameter() {
  auto name = parseIdentifier();
  auto result = alloc<ast::TypeParameter>();

  if(poll(Token::kRArrow)) {
    auto bound = parseType();
    result->init(name, bound, 1);
  } else if(poll(Token::kLArrow)) {
    auto bound = parseType();
    result->init(name, bound, 2);
  } else {
    result->init(name, nullptr, 0);
  }

  return result;
}

//
// Identifier
//  : IDENTIFIER
//
ast::Node* Parser::parseIdentifier() {
  return consume(Token::kIdentifier, [&]() {
    return allocWithValue<ast::Identifier>();
  });
}

// Utility methods for the parser
//

template<class T>
T* Parser::alloc() {
  return new (m_arena) T();
}

template<class T>
T* Parser::allocWithValue() {
  // Allocate a buffer with the given value's length.
  // We add +1 to that length to store a terminating 0
  // character.

  auto result = alloc<T>();
  auto name = m_names->get(m_lexer->value(), m_lexer->valueLength(), /*copyValue=*/YES);

  result->init(name);

  return result;
}

ast::Node* Parser::error(const char* value) {
  auto result = alloc<ast::Error>();
  result->init(value, m_lexer->posLine(), m_lexer->posColumn());
  return result;
}

inline bool Parser::peek(const Token& token) {
  return m_currentToken == token;
}

inline bool Parser::poll(const Token& token) {
  if(peek(token)) {
    advance();
    return YES;
  }

  return NO;
}

void Parser::pollAll(const Token& token) {
  while(poll(token));
}

ast::Node* Parser::consume(
    const Token& token,
    std::function<ast::Node*()> f) { //NOLINT
  if(peek(token)) {
    auto result = f();
    advance();
    return result;
  }

  //TODO(joa): build more meaningful error message.
  advance();

  return error("Error: Unexpected token.");
}

void Parser::advance() {
  do {
    m_currentToken = m_lexer->nextToken();
  } while(isIgnored(m_currentToken));
}

bool Parser::isIgnored(const Token& token) {
  switch(token) {
    case Token::kWhitespace:
    case Token::kCommentSingle:
    case Token::kCommentMulti:
      return YES;
    default:
      return NO;
  }
}
} //namespace internal
} //namespace brutus
