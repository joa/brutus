#include "brutus.h"

namespace brutus {
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
// Then there are 0 ore more repitions of a newline token followed by an Expression.
// This is followed by a final RBRACE token.
//
// The second case is just a single Expression.
//

// Macro to expect a certain token type. If no such token is found
// the ast::Error token is returned.

//TODO(joa): arena
#define EXPECT(t) if(!poll(t)) { \
  auto buf = NewArray<char>(0x100); \
  sprintf(buf, "%s:%d Invalid syntax. Expected %s, got %s.", __FILE__, __LINE__,tok::toString(t), tok::toString(m_currentToken)); \
  return error(buf); \
}

//
// Program
//  : Block NEWLINE
//
ast::Node* Parser::parseProgram() {
  advance();

  auto result = parseBlock();
  EXPECT(tok::NEWLINE);

  return result;
}

//
// Block 
//  : '{' NEWLINE (Expression NEWLINE)+ '}'
//  | Expression
// 
ast::Node* Parser::parseBlock() {
  if(poll(tok::LBRACE)) {
    EXPECT(tok::NEWLINE);
    auto block = alloc<ast::Block>();

    do {
      block->add(parseBlock());
      EXPECT(tok::NEWLINE);
    } while(!peek(tok::RBRACE));

    EXPECT(tok::RBRACE);
    return block;
  } else {
    return parseExpression();
  }
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

//
// Expression
//  : NEWLINE* '(' Expression ')'
//  | NEWLINE* Identifier
//  | NEWLINE* VariableExpression
//  | NEWLINE* IfExpression
//  | NEWLINE* NumberLiteral
//  | NEWLINE* BooleanLiteral
//  | NEWLINE* StringLiteral
//  | NEWLINE* 'this'
//  | NEWLINE* Select
//  | NEWLINE* Call
//
ast::Node* Parser::parseExpression() {
  pollAll(tok::NEWLINE);

  ast::Node* expression = nullptr;

  if(poll(tok::LPAREN)) {
    expression = parseExpression();
    EXPECT(tok::RPAREN);
  } else if(peek(tok::IDENTIFIER)) {
    expression = parseIdentifier();
  } else if(peek(tok::IF)) {
    expression =  parseIfExpression();
  } else if(peek(tok::VAL) || peek(tok::VAR)) {
    expression =  parseVariableExpression();
  } else if(peek(tok::NUMBER_LITERAL)) {
    expression =  parseNumberLiteral();
  } else if(peek(tok::TRUE_) || peek(tok::YES_) || peek(tok::FALSE_) || peek(tok::NO_)) {
    expression =  parseBooleanLiteral();
  } else if(peek(tok::STRING_LITERAL)) {
    expression =  parseStringLiteral();
  } else if(poll(tok::THIS)) {
    expression =  alloc<ast::This>();
  } 

  return continueWithExpression(expression);
}

ast::Node* Parser::continueWithExpression(ast::Node* expression) {
  if(nullptr == expression) {
    return error("Expected expression.");
  } else {
    while(true) {
      if(peek(tok::DOT)) {
        auto select = parseSelect(expression);
        expression = select;
      } else if(peek(tok::LPAREN) || peek(tok::IDENTIFIER)) {
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
//  : Expression '(' Arguments ')'
//  | Expression Identifier Argument
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

    result->arguments()->add(parseArgument());
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
    list->add(parseArgument());
  } while(poll(tok::COMMA));
}

//
// Argument
//  : Expression
//  | Identifier '=' Expression
//
ast::Node* Parser::parseArgument() {
  auto result = alloc<ast::Argument>();

  if(peek(tok::IDENTIFIER)) {
    auto ident = parseIdentifier();

    if(poll(tok::ASSIGN)) {
      auto value = parseExpression();
      result->init(ident, value);
    } else {
      result->init(nullptr, continueWithExpression(ident));
    }
  } else {
    result->init(nullptr, parseExpression());
  }

  return result;
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
  return consume(tok::NUMBER_LITERAL, [&]() { return allocWithValue<ast::Number>(); });
}

//
// StringLiteral
//  : STRING_LITERAL
//
ast::Node* Parser::parseStringLiteral() {
  return consume(tok::STRING_LITERAL, [&]() { return allocWithValue<ast::String>(); });
}

//
// IfExpression
//  : 'if' '(' Expression ')' Block ('else' Block)?
//
ast::Node* Parser::parseIfExpression() {
  EXPECT(tok::IF);
  EXPECT(tok::LPAREN);
  auto condition = parseExpression();
  EXPECT(tok::RPAREN);
  
  auto trueCase = parseBlock();
  auto falseCase = poll(tok::ELSE) ? parseBlock() : nullptr;
  auto result = alloc<ast::If>();

  result->init(condition, trueCase, falseCase);

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

// Type
//  : Identifier ('[' Type ']')?
//
ast::Node* Parser::parseType() {
  //TODO(joa): wrap in type, add attrib
  return parseIdentifier();
}

//
// Identifier
//  : IDENTIFIER
//
ast::Node* Parser::parseIdentifier() {
  return consume(tok::IDENTIFIER, [&]() { return allocWithValue<ast::Identifier>(); });
}

// Utility methods for the parser
//

template<class T>
T* Parser::alloc() {
  //TODO(joa): alloc in arena
  return new T();
}

template<class T>
T* Parser::allocWithValue() {
  auto result = alloc<T>();
  result->copyValue(m_lexer);
  return result;
}

ast::Node* Parser::error(const char* value) {
  auto result = alloc<ast::Error>();
  result->init(value, m_lexer->posLine(), m_lexer->posColumn());
  return result;
}

bool Parser::peek(const tok::Token& token) {
  return m_currentToken == token;
}

bool Parser::poll(const tok::Token& token) {
  if(peek(token)) {
    advance();
    return YES;
  }

  return NO;
}

void Parser::pollAll(const tok::Token& token) {
  while(poll(token));
}

ast::Node* Parser::consume(const tok::Token& token, std::function<ast::Node*()> f) {
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
} //namespace brutus
