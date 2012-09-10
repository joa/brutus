#include "brutus.h"

namespace brutus {
//
// Comments define the Brutus grammar in something similar to
// EBNF. The following conventions are used:
//
//   *     = 0 to n repitions
//   +     = 1 to n repititions
//   ()    = Sequence
//   ?     = Optional
//   UPPER = Token type
//   Lower = Production rule
//   '{'   = Shorthand for token type LBRACE for instance
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
#define EXPECT(t) if(!accept(t)) { return error(u8"Invalid syntax. Expected " #t "."); }

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
  if(accept(tok::LBRACE)) {
    EXPECT(tok::NEWLINE);

    auto block = create<ast::Block>();

    do {
      block->add(parseExpression());
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
// ActorName
//  : Identifier
//

//
// Path
//   : '.' ('/' Path)?
//  | '..' ('/' Path)?
//  | '/' Path
//  | ActorName ('/' Path)?
//

//
// Expression
//  : Call
//  | ActorPath
//
ast::Node* Parser::parseExpression() {
  return parseIdentifier();
}

//
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
  return consume(tok::IDENTIFIER, [&]() { return createWithValue<ast::Identifier>(); });
}

template<class T>
T* Parser::create() {
  //TODO(joa): alloc in arena
  return new T();
}

template<class T>
T* Parser::createWithValue() {
  auto result = create<T>();
  result->copyValue(m_lexer);
  return result;
}

ast::Node* Parser::error(const char* value) {
  auto result = create<ast::Error>();
  result->init(value, m_lexer->posLine(), m_lexer->posColumn());
  return result;
}

bool Parser::peek(const tok::Token& token) {
  return m_currentToken == token;
}

bool Parser::accept(const tok::Token& token) {
  if(peek(token)) {
    advance();
    return YES;
  }

  return NO;
}

ast::Node* Parser::consume(const tok::Token& token, std::function<ast::Node*()> f) {
  if(peek(token)) {
    auto result = f();
    advance();
    return result;
  }

  //TODO(joa): build more meaningful error message.
  advance();

  return error(u8"Error: Unexpected token.");
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
