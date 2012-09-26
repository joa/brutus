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
    if(peek(tok::DEF)) {
      return parseDefinition();
    }

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

// this is inconsistent since '{' Block '}' is not true. '{' and '}' are
// already part of the block but they must be present!
//
// Definition
//  : 'def' Identifier TypeParameterList? '(' ParameterList ')' Type '=' Block
//  | 'def' Identifier TypeParameterList? '(' ParameterList ')' '{' Block '}'
//
ast::Node* Parser::parseDefinition() {
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
    parseParameterList(result->parameters());
  }
  
  EXPECT(tok::RPAREN);
  
  if(poll(tok::COLON)) {
    auto type = parseType();
    EXPECT(tok::ASSIGN);
    auto block = parseBlock();
    result->init(name, type, block);
  } else if(peek(tok::LBRACE)) {
    auto block = parseBlock();
    result->init(name, nullptr, block);
  }

  return result;
}

void Parser::parseParameterList(ast::NodeList* list) {
  do {
    list->add(parseParameter());
  } while(poll(tok::COMMA));
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
// Expression
//  : NEWLINE* '(' Expression ')'
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

  if(poll(tok::LPAREN)) {
    expression = parseExpression();
    EXPECT(tok::RPAREN);
  } else if(peek(tok::BRANCH)) {
    expression = parseBranchExpression();
  } else if(peek(tok::VAL) || peek(tok::VAR)) {
    expression =  parseVariableExpression();
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
  if(peek(tok::IDENTIFIER)) {
    return parseIdentifier();
  } else if(peek(tok::NUMBER_LITERAL)) {
    return parseNumberLiteral();
  } else if(peek(tok::TRUE_) || peek(tok::YES_) || peek(tok::FALSE_) || peek(tok::NO_)) {
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

ast::Node* Parser::continueWithExpression(ast::Node* expression, bool allowInfixCall) {
  if(nullptr == expression) {
    return error("Expected expression.");
  } else {
    while(true) {
      if(peek(tok::DOT)) {
        auto select = parseSelect(expression);
        expression = select;
      } else if(peek(tok::LPAREN) || (allowInfixCall && peek(tok::IDENTIFIER))) {
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

    result->arguments()->add(parseSingleArgument());
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
//  : '{' NEWLINE* AnonymousFunctionParameterList '->' Block NEWLINE* '}'
//
ast::Node* Parser::parseAnonymousFunctionExpression() {
  EXPECT(tok::LBRACE);
  pollAll(tok::NEWLINE);
  auto result = alloc<ast::Function>();
  parseAnonymousFunctionParameterList(result->parameters());
  EXPECT(tok::RARROW);
  result->init(nullptr, nullptr, parseBlock());
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
    parameters->add(parseAnonymousFunctionParameter());
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
// BranchExpression
//  : branch '{' NEWLINE (BranchCase NEWLINE)+ '}'
//  | branch BranchCase
ast::Node* Parser::parseBranchExpression() {
  auto branch = alloc<ast::Branch>();
  
  EXPECT(tok::BRANCH);
  if(poll(tok::LBRACE)) {
    EXPECT(tok::NEWLINE);
    auto cases = branch->cases();
    do {
      cases->add(parseBranchCase());
      EXPECT(tok::NEWLINE);
    } while(!peek(tok::RBRACE));
    EXPECT(tok::RBRACE);
  } else {
    branch->cases()->add(parseBranchCase());
  }

  return branch;
}

//
// BranchCase
//  : Expression '->' Block
//
ast::Node* Parser::parseBranchCase() {
  auto condition = parseExpression();
  EXPECT(tok::RARROW);
  auto block = parseBlock();
  auto result = alloc<ast::BranchCase>();
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
//
ast::Node* Parser::parseType() {
  //TODO(joa): wrap in type, add attrib
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

//
// TypeParameterList
//  : '[' TypeParameter (',' TypeParameter)* ']'
//
ast::Node* Parser::parseTypeParameterList(ast::NodeList* list) {
  EXPECT(tok::LBRAC);
  
  do {
    list->add(parseTypeParameter());
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

ALWAYS_INLINE bool Parser::peek(const tok::Token& token) {
  return m_currentToken == token;
}

ALWAYS_INLINE bool Parser::poll(const tok::Token& token) {
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
