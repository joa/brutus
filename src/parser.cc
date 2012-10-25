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
#define EXPECT(t) if(!poll(t)) { \
  auto buf = m_arena->newArray<char>(0x100); \
  _snprintf(buf, 0x100, "%s:%d Invalid syntax. Expected %s, got %s.", __FILE__, \
    __LINE__, tok::toString(t), tok::toString(m_currentToken)); \
  return error(buf); \
}
#else
#define EXPECT(t) if(!poll(t)) { \
  auto buf = m_arena->newArray<char>(0x100); \
  snprintf(buf, 0x100, "%s:%d Invalid syntax. Expected %s, got %s.", __FILE__, \
    __LINE__, tok::toString(t), tok::toString(m_currentToken)); \
  return error(buf); \
}
#endif

//
// Program
//  : (Module NEWLINE)*
//
ast::Node* Parser::parseProgram() {
  advance();

  auto result = alloc<ast::Program>();
  auto modules = result->modules();

  while(peek(tok::MODULE)) {
    modules->add(parseModule(), m_arena);
    EXPECT(tok::NEWLINE);
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
  EXPECT(tok::MODULE);

  auto result = alloc<ast::Module>();
  ast::Node* name = nullptr;

  if(peek(tok::IDENTIFIER)) {
     name = parseIdentifier(); //TODO(joa): parseActorName()
  } else {
    auto emptyName = alloc<ast::Identifier>();
    emptyName->init(m_names->get("", 0, /*copyValue=*/false));

    name = emptyName;
  }

  result->init(name);

  EXPECT(tok::LBRACE);
  EXPECT(tok::NEWLINE);

  auto dependencies = result->dependencies();
  auto declarations = result->declarations();

  do {
    // ModuleDeclaration
    if(peek(tok::REQUIRE)) {
      dependencies->add(parseModuleDependency(), m_arena);
    } else {
      ast::Node* declaration = parseDeclaration();
    
      if(nullptr == declaration) {
        declarations->add(parseExpression(), m_arena);
      } else {
        declarations->add(declaration, m_arena);
      }
    }
    EXPECT(tok::NEWLINE);
  } while(!peek(tok::RBRACE));

  EXPECT(tok::RBRACE);

  return result;
}

//
// ModuleDependency
//  : 'require' ActorName ModuleVersion?
//  ;

ast::Node* Parser::parseModuleDependency() {
  EXPECT(tok::REQUIRE);

  auto result = alloc<ast::ModuleDependency>();
  auto name = parseIdentifier(); //TODO(joa): parseActorName()

  //TODO(joa):
  result->init(name, peek(tok::COLON) ? parseModuleVersion() : nullptr);

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
  EXPECT(tok::COLON);

  bool isBracket;

  if(poll(tok::LBRAC)) {
    isBracket = YES;
  } else if(poll(tok::LPAREN)) {
    isBracket = NO;
  } else {
    auto number = parseNumberLiteral();
    //TODO(joa): NumberLiteral
    return error("TODO NumberLiteral");
  }

  if(poll(tok::COMMA)) {
    // ?,
    auto x = parseNumberLiteral();

    if(poll(tok::RBRAC)) {
      //?,x]
      return error("TODO ?,x]");
    } else if(poll(tok::RPAREN)) {
      //?,x)
      return error("TODO ?,x)");
    } else {
      return error("Expected ']' or ')'");
    }
  } else {
    // ?x
    auto x = parseNumberLiteral();

    if(poll(tok::COMMA)) {
      //[x,
      if(poll(tok::RBRAC)) {
        // [x,]
        return error("TODO ?x,]");
      } else if(poll(tok::RPAREN)) {
        // [x,)
        return error("TODO ?x,)");
      } else {
        //[x,y
        auto y = parseNumberLiteral();

        if(poll(tok::RBRAC)) {
          //[x,y]
          return error("TODO ?x,y]");
        } else if(poll(tok::RPAREN)) {
          //[x,y)
          return error("TODO ?x,y)");
        } else {
          return error("Expected ']' or ')'");
        }
      }
    } else if(isBracket && poll(tok::RBRAC)) {
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
  if(poll(tok::LBRACE)) {
    EXPECT(tok::NEWLINE);
    auto block = alloc<ast::Block>();
    auto expressions = block->expressions();

    do {
      expressions->add(parseBlock(), m_arena);
      EXPECT(tok::NEWLINE);
    } while(!peek(tok::RBRACE));

    EXPECT(tok::RBRACE);
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
//  | Trait
//  ;
//
ast::Node* Parser::parseDeclaration() {
  if(peek(tok::DEF)) {
    return parseFunction(Parser::ACC_PUBLIC);
  } else if(peek(tok::CLASS)) {
    return parseClass(Parser::ACC_PUBLIC);
  } else if(peekVisibility()) {
    unsigned int flags = 0;

    if(poll(tok::PUBLIC)) {
      flags |= Parser::ACC_PUBLIC;
    } else if(poll(tok::PRIVATE)) {
      flags |= Parser::ACC_PRIVATE;
    } else if(poll(tok::PROTECTED)) {
      flags |= Parser::ACC_PROTECTED;
    } else if(poll(tok::INTERNAL)) {
      flags |= Parser::ACC_INTERNAL;
    }

    if(poll(tok::VIRTUAL)) {
      flags |= Parser::ACC_VIRTUAL;
    }

    if(poll(tok::NATIVE)) {
      flags |= Parser::ACC_NATIVE;
    }

    if(peek(tok::DEF)) {
      return parseFunction(flags);
    } else if(peek(tok::CLASS)) {
      return parseClass(flags);
    } else {
      return error("Expected 'def' or 'class'.");
    }
  } else if(poll(tok::VIRTUAL)) {
    // "virtual class" is the same as "public virtual class"
    unsigned int flags = Parser::ACC_PUBLIC | Parser::ACC_VIRTUAL; 

    if(poll(tok::NATIVE)) {
      flags |= Parser::ACC_NATIVE;
    }

    if(peek(tok::DEF)) {
      return parseFunction(flags);
    } else if(peek(tok::CLASS)) {
      return parseClass(flags);
    } else {
      return error("Expected 'def' or 'class'.");
    }
  } else {
    return nullptr;
  }
}

bool Parser::peekVisibility() {
  return peek(tok::PUBLIC) 
      || peek(tok::PRIVATE) 
      || peek(tok::PROTECTED) 
      || peek(tok::INTERNAL);
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

  EXPECT(tok::CLASS);

  auto name = parseIdentifier();
  auto result = alloc<ast::Class>();

  if(peek(tok::LBRAC)) {
    auto error = parseTypeParameterList(result->typeParameters());
    
    if(nullptr != error) {
      return error;
    }
  }

  if(poll(tok::LPAREN)) {
    //TODO(joa): implement ctor
    EXPECT(tok::RPAREN);
  }


  if(poll(tok::COLON)) {
    //...
  }

  if(poll(tok::LBRACE)) {
    EXPECT(tok::NEWLINE);

    auto members = result->members();
    
    while(!poll(tok::RBRACE)) {
      pollAll(tok::NEWLINE);
      members->add(parseDeclaration(), m_arena);
      EXPECT(tok::NEWLINE);
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
  EXPECT(tok::DEF);
  
  auto name = parseIdentifier();
  auto result = alloc<ast::Function>();

  if(peek(tok::LBRAC)) {
    auto error = parseTypeParameterList(result->typeParameters());
    
    if(nullptr != error) {
      return error;
    }
  }
  
  EXPECT(tok::LPAREN);
  
  if(!peek(tok::RPAREN)) {
    if(!parseParameterList(result->parameters())) {
      return error("Function exceeds max arity.");
    }
  }
  
  EXPECT(tok::RPAREN);
  
  ast::Node* block = nullptr;
  ast::Node* type = nullptr;

  if(poll(tok::COLON)) {
    type = parseType();
    
    if(poll(tok::ASSIGN)) {
      block = parseBlock();
    } else if(peek(tok::NEWLINE)) {
      flags |= ACC_ABSTRACT;
    } else {
      return error("Expected '=' or '\n'.");
    }
  } else if(peek(tok::LBRACE)) {
    block = parseBlock();
  } else if(peek(tok::NEWLINE)) {
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
  } while(poll(tok::COMMA));

  return true;
}

ast::Node* Parser::parseParameter() {
  auto name = parseIdentifier();
  EXPECT(tok::COLON);
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
//  | NEWLINE* VariableExpression
//  | NEWLINE* IfExpression
//  | NEWLINE* Expression Select*
//  | NEWLINE* Expression Call*
//  | NEWLINE* PrimaryExpression
//
ast::Node* Parser::parseExpression() {
  return parseExpression(true);
}

ast::Node* Parser::parseExpression(bool allowInfixCall) {
  pollAll(tok::NEWLINE);

  ast::Node* expression = nullptr;

  //bool isForced = poll(tok::FORCE);

  if(poll(tok::LPAREN)) {
    expression = parseExpression();
    
    if(peek(tok::COMMA)) {
      int arity = 1;

      do {
        EXPECT(tok::COMMA);
        
        parseExpression();
        
        ++arity;

        if(arity > consts::MaxTupleArity) {
          return error("Tuple exceeds max arity.");
        }
      } while(!poll(tok::RPAREN));
    } else {
      EXPECT(tok::RPAREN);
    }
  } else if(peek(tok::IF)) {
    expression = parseIfExpression();
  } else if(peek(tok::VAL) || peek(tok::VAR)) {
    expression =  parseVariableExpression();
  } else if(peekPrimaryExpression()) {
    expression = parsePrimaryExpression();
  }

  //if(nullptr != expression) {
  //  expression->force(isForced);
  //}

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
  if(peek(tok::IDENTIFIER)) {
    return parseIdentifier();
  } else if(peek(tok::NUMBER_LITERAL)) {
    return parseNumberLiteral();
  } else if(peek(tok::TRUE_) || peek(tok::YES_) || 
            peek(tok::FALSE_) || peek(tok::NO_)) {
    return parseBooleanLiteral();
  } else if(peek(tok::STRING_LITERAL)) {
    return parseStringLiteral();
  } else if(peek(tok::THIS)) {
    return parseThis();
  } else {
    return error("Expected primary expression.");
  }
}

bool Parser::peekPrimaryExpression() {
  return peek(tok::IDENTIFIER)
      || peek(tok::NUMBER_LITERAL)
      || peek(tok::TRUE_)
      || peek(tok::YES_)
      || peek(tok::FALSE_)
      || peek(tok::NO_)
      || peek(tok::STRING_LITERAL)
      || peek(tok::THIS);
}

ast::Node* Parser::continueWithExpression(
    ast::Node* expression, bool allowInfixCall) {
  if(nullptr == expression) {
    return error("Expected expression.");
  } else {
    while(true) {
      if(peek(tok::DOT)) {
        auto select = parseSelect(expression);
        expression = select;
      } else if(peek(tok::LPAREN) || 
          (allowInfixCall && peek(tok::IDENTIFIER))) {
        auto call = parseCall(expression);
        expression = call;
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
  EXPECT(tok::DOT);

  auto qualifier = parseIdentifier();
  auto result = alloc<ast::Select>();
  
  result->init(object, qualifier);

  return result;
}

//
// Call
//  : Expression '(' ArgumentList ')'
//  | Expression Identifier SingleArgument
//
ast::Node* Parser::parseCall(ast::Node* callee) {
  auto result = alloc<ast::Call>();

  if(poll(tok::LPAREN)) {
    if(!peek(tok::RPAREN)) {
      parseArgumentList(result->arguments());
    }
    
    EXPECT(tok::RPAREN);

    result->init(callee);
  } else if(peek(tok::IDENTIFIER)) {
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
  } while(poll(tok::COMMA));
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

  if(peek(tok::IDENTIFIER)) {
    auto ident = parseIdentifier();

    if(poll(tok::ASSIGN)) {
      if(peek(tok::LBRACE)) {
        result->init(ident, parseAnonymousFunctionExpression());
      } else {
        result->init(ident, parseExpression());
      }
    } else {
      result->init(nullptr, continueWithExpression(ident, true));
    }
  } else {
    if(peek(tok::LBRACE)) {
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

  if(peek(tok::LBRACE)) {
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
  EXPECT(tok::LBRACE);
  pollAll(tok::NEWLINE);
  auto result = alloc<ast::Function>();
  parseAnonymousFunctionParameterList(result->parameters());
  ast::Node* returnType = nullptr;
  if(poll(tok::COLON)) {
    returnType = parseType();
  }
  EXPECT(tok::RARROW);
  result->init(nullptr, returnType, parseBlock(), ACC_PUBLIC);
  pollAll(tok::NEWLINE);
  EXPECT(tok::RBRACE);
  return result;
}

//
// AnonymousFunctionParameterList
//  : AnonymousFunctionParameter (',' AnonymousFunctionParameter)*
//
void Parser::parseAnonymousFunctionParameterList(ast::NodeList* parameters) {
  do {
    parameters->add(parseAnonymousFunctionParameter(), m_arena);
  } while(poll(tok::COMMA));
}

//
// AnonymousFunctionParameter
//  : Identifier (':' Type)?
//
ast::Node* Parser::parseAnonymousFunctionParameter() {
  auto ident = parseIdentifier();
  auto type = poll(tok::COLON) ? parseType() : nullptr;
  auto result = alloc<ast::Parameter>();
  result->init(ident, type);
  return result;
}

//
// This
//  : 'this'
//
ast::Node* Parser::parseThis() {
  if(poll(tok::THIS)) {
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
  if(poll(tok::TRUE_) || poll(tok::YES_)) {
    return alloc<ast::True>();
  } else if(poll(tok::FALSE_) || poll(tok::NO_)) {
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
  return consume(tok::NUMBER_LITERAL, [&]() {
    return allocWithValue<ast::Number>();
  });
}

//
// StringLiteral
//  : STRING_LITERAL
//
ast::Node* Parser::parseStringLiteral() {
  return consume(tok::STRING_LITERAL, [&]() {
    return allocWithValue<ast::String>();
  });
}

//
// IfExpression
//  : if '{' NEWLINE (IfCase NEWLINE)+ '}'
//  | if IfCase
ast::Node* Parser::parseIfExpression() {
  auto result = alloc<ast::If>();
  
  EXPECT(tok::IF);
  if(poll(tok::LBRACE)) {
    EXPECT(tok::NEWLINE);
    auto cases = result->cases();
    do {
      cases->add(parseIfCase(), m_arena);
      EXPECT(tok::NEWLINE);
    } while(!peek(tok::RBRACE));
    EXPECT(tok::RBRACE);
  } else {
    result->cases()->add(parseIfCase(), m_arena);
  }

  return result;
}

//
// IfCase
//  : Expression '->' Block
//
ast::Node* Parser::parseIfCase() {
  auto condition = parseExpression();
  EXPECT(tok::RARROW);
  auto block = parseBlock();
  auto result = alloc<ast::IfCase>();
  result->init(condition, block);
  return result;
}

//
// VariableExpression
//  : ('val' | 'var') Identifier (((':' Type)? '=' Expression) | ':' Type)
//
ast::Node* Parser::parseVariableExpression() {
  bool modifiable = NO;

  if(poll(tok::VAR)) {
    modifiable = YES;
  } else if(poll(tok::VAL)) {
    modifiable = NO;
  } else {
    return error("Expected \"var\" or \"val\".");
  }

  auto name = parseIdentifier();
  ast::Node* type = nullptr;

  if(poll(tok::COLON)) {
    // Optional type is present.
    type = parseType();
  }

  ast::Node* init = nullptr;

  if(poll(tok::ASSIGN)) {
    init = parseExpression();
  }

  if(nullptr == init && nullptr == type) {
    return error("Either a type or initializer must be given.");
  }

  auto result = alloc<ast::Variable>();
  
  result->init(modifiable, name, type, init);

  return result;
}

//
// Type
//  : Identifier ('[' Type ']')?
//  | Type '->' Type
//  | '(' Type (',' Type)+ ')'
//
ast::Node* Parser::parseType() {
  //TODO(joa): wrap in type, add attrib
  if(poll(tok::LPAREN)) {
    int arity = 1;

    parseType();

    do {
      EXPECT(tok::COMMA);
      parseType();
      ++arity;

      if(arity > consts::MaxTupleArity) {
        return error("Tuple exceeds max arity.");
      }
    } while(!poll(tok::RPAREN));

    return nullptr;
  } else {
    auto name = parseIdentifier();

    if(poll(tok::LBRAC)) {
      parseType();
      EXPECT(tok::RBRAC);
    }

    if(poll(tok::RARROW)) {
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
  EXPECT(tok::LBRAC);
  
  do {
    list->add(parseTypeParameter(), m_arena);
  } while(poll(tok::COMMA));

  EXPECT(tok::RBRAC);

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

  if(poll(tok::RARROW)) {
    auto bound = parseType();
    result->init(name, bound, 1);
  } else if(poll(tok::LARROW)) {
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
  return consume(tok::IDENTIFIER, [&]() {
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

inline bool Parser::peek(const tok::Token& token) {
  return m_currentToken == token;
}

inline bool Parser::poll(const tok::Token& token) {
  if(peek(token)) {
    advance();
    return YES;
  }

  return NO;
}

void Parser::pollAll(const tok::Token& token) {
  while(poll(token));
}

ast::Node* Parser::consume(
    const tok::Token& token,
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

bool Parser::isIgnored(const tok::Token& token) {
  switch(token) {
    case tok::WHITESPACE:
    case tok::COMMENT_SINGLE:
    case tok::COMMENT_MULTI:
      return YES;
    default:
      return NO;
  }
}
} //namespace internal
} //namespace brutus
