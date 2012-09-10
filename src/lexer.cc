#include "brutus.h"

namespace brutus {
namespace tok {
auto toString(const Token& token) -> const char* {
  #define TOKEN_TO_STRING_CASE(T, S) case T: return S
  switch(token) {
    TOKEN_TO_STRING_CASE(_EOF, u8"EOF");
    TOKEN_TO_STRING_CASE(ERROR, u8"ERROR");
    TOKEN_TO_STRING_CASE(NEWLINE, u8"NEWLINE");
    TOKEN_TO_STRING_CASE(WHITESPACE, u8"WHITESPACE");

    TOKEN_TO_STRING_CASE(IDENTIFIER, u8"IDENTIFIER");

    TOKEN_TO_STRING_CASE(NUMBER_LITERAL, u8"NUMBER_LITERAL");
    TOKEN_TO_STRING_CASE(BOOLEAN_LITERAL, u8"BOOLEAN_LITERAL");
    TOKEN_TO_STRING_CASE(STRING_LITERAL, u8"STRING_LITERAL");

    TOKEN_TO_STRING_CASE(SEMICOLON, u8"SEMICOLON");
    TOKEN_TO_STRING_CASE(COMMA, u8"COMMA");
    TOKEN_TO_STRING_CASE(LPAREN, u8"LPAREN");
    TOKEN_TO_STRING_CASE(RPAREN, u8"RPAREN");
    TOKEN_TO_STRING_CASE(LBRAC, u8"LBRAC");
    TOKEN_TO_STRING_CASE(RBRAC, u8"RBRAC");
    TOKEN_TO_STRING_CASE(LBRACE, u8"LBRACE");
    TOKEN_TO_STRING_CASE(RBRACE, u8"RBRACE");
    TOKEN_TO_STRING_CASE(DOT, u8"DOT");
    TOKEN_TO_STRING_CASE(COLON, u8"COLON");

    TOKEN_TO_STRING_CASE(PLUS, u8"PLUS");
    TOKEN_TO_STRING_CASE(MINUS, u8"MINUS");
    TOKEN_TO_STRING_CASE(ASTERISK, u8"ASTERISK");
    TOKEN_TO_STRING_CASE(SLASH, u8"SLASH");
    TOKEN_TO_STRING_CASE(PERCENT, u8"PERCENT");

    default: return u8"UNKNOWN";
  }
  #undef TOKEN_TO_STRING_CASE
}

auto hasValue(const Token& token) -> bool {
  switch(token) {
    case IDENTIFIER:
    case NUMBER_LITERAL:
    case BOOLEAN_LITERAL:
    case STRING_LITERAL:
      return YES;
    default:
      return NO;
  }
}
} //namespace tok

auto Lexer::resulting(
    std::function<bool(const char)> condition,
    std::function<bool(const char)> sideEffect,
    tok::Token result) -> tok::Token {
  while(canAdvance()) {
    const auto nextChar = advance();

    if(!condition(nextChar)) {
      rewind();
      break;
    } else {
      if(!sideEffect(nextChar)) {
        return tok::ERROR;
      }
    }
  }

  return result;
}

auto Lexer::nextToken() -> tok::Token {
  while(canAdvance()) {
    auto currentChar = advance();

    if(isWhitespace(currentChar)) {
      return resulting(
        [&](const char c) { return isWhitespace(c); },
        [&](const char) { return YES; },
        tok::WHITESPACE
      );
    } else if(isNewLine(currentChar)) {
      ++m_line;

      return resulting(
        [&](const char c) { return isNewLine(c); },
        [&](const char) -> bool { ++m_line; return YES; },
        tok::NEWLINE
      );
    } else if(isNumberStart(currentChar)) {
      return continueWithNumberStart(currentChar);
    } else if(isIdentifierStart(currentChar)) {
      return continueWithIdentifierStart(currentChar);
    } else if('/' == currentChar) {
      return continueWithSlash(currentChar);
    } else {
      switch(currentChar) {
        case ';': return tok::SEMICOLON;
        case ',': return tok::COMMA;
        case '(': return tok::LPAREN;
        case ')': return tok::RPAREN;
        case '[': return tok::LBRAC;
        case ']': return tok::RBRAC;
        case '{': return tok::LBRACE;
        case '}': return tok::RBRACE;
        case '.': return tok::DOT;
        case ':': return tok::COLON;

        case '+': return tok::PLUS;
        case '-': return tok::MINUS;
        case '*': return tok::ASTERISK;
        case '%': return tok::PERCENT;

        default:
          return tok::ERROR;
      }
    }
  }

  return tok::_EOF;
}

auto Lexer::value() -> const char* {
  return m_buffer;
}

auto Lexer::valueLength() -> size_t {
  return m_bufferIndex;
}

auto Lexer::posLine() -> int {
  return m_line;
}

auto Lexer::posColumn() -> int {
  return m_column;
}

auto Lexer::canAdvance() -> bool {
  return m_advanceWithLastChar || m_stream->hasNext();
}

auto Lexer::advance() -> char {
  if(m_advanceWithLastChar) {
    m_advanceWithLastChar = NO;
  } else {
    m_currentChar = m_stream->next();
  }

  ++m_column;
  
  return m_currentChar;
}

auto Lexer::rewind() -> void {
#ifdef DEBUG
  if(m_advanceWithLastChar) {
    std::cout << u8"Error: Called Lexer::rewind() twice in a row." << std::endl;
  }
#endif

  m_advanceWithLastChar = YES;
  --m_column;
}

auto Lexer::isWhitespace(const char c) -> bool {
  // Note that we explicitly forbid \t in Brutus
  // source code so only space characters are treated
  // as whitespace.
  return c == ' ';
}

auto Lexer::isNewLine(const char c) -> bool {
  // Note that we explicitly forbid \r in Brutus
  // source code so only \n is allowed as a line
  // terminator.
  return c == '\n';
}

auto Lexer::isNumberStart(const char c) -> bool {
  return c >= '0' && c <= '9';
}

auto Lexer::isIdentifierStart(const char c) -> bool {
  return (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z')
      || (c == '_');
}

auto Lexer::isIdentifierPart(const char c) -> bool {
  return isIdentifierStart(c)
      || (c >= '0' && c <= '9');
}

auto Lexer::isDigit(const char c) -> bool {
  return (c >= '0' && c <= '9');
}

//

auto Lexer::continueWithNumberStart(const char currentChar) -> tok::Token {
  beginBuffer(currentChar);

  // We allow only 0xffffff not 0XFFFFFF, not 0xFFFFFF
  const auto secondChar = advance();

  if(secondChar == 'x') {
    // Parse hexadecimal literal
    while(canAdvance()) {
      const auto hexChar = advance();

      if(isDigit(hexChar) || (hexChar >= 'a' && hexChar <= 'f')) {

      } else {
        rewind();
        break;
      }
    }
  } else if(isDigit(secondChar) || secondChar == 'e' || secondChar == '.') {
    // Parse a digit of one of the following forms:
    //
    // 12 ...
    // 1e ...
    // 1. ...
  }

  // We can also do something like this to 
  // check for a long value.
  //
  // if(advance() == 'L') {
  //
  // }

  return tok::NUMBER_LITERAL;
}

auto Lexer::continueWithIdentifierStart(const char currentChar) -> tok::Token {
  beginBuffer(currentChar);

  return resulting(
    [&](const char c) { return isIdentifierPart(c); },
    [&](const char c) { return continueBuffer(c); },
    tok::IDENTIFIER
  );
}

auto Lexer::continueWithSlash(const char currentChar) -> tok::Token {
  beginBuffer(currentChar);

  const auto nextChar = advance(); 

  if(nextChar == '/') {
    continueBuffer(nextChar);

    while(canAdvance()) {
      char singleLineChar = advance();

      if(isNewLine(singleLineChar)) {
        break;
      }

      continueBuffer(singleLineChar);
    }

    return tok::COMMENT_SINGLE;
  } else if(nextChar == '*') {
    continueBuffer(nextChar);

    auto openComments = 1;
    
    while(openComments > 0 && canAdvance()) {
      const auto commentChar = advance();

      if(commentChar == '/') {
        //TODO(joa): check for opening comment.

      } else if(commentChar == '*') {
        if(advance() == '/') {
          --openComments;
        } else {
          rewind();
        }
      } else {
        continueBuffer(commentChar);
      }
    }

    if(openComments > 0) {
      // Reached premature EOF.
      // We can ignore this if we do not want to be
      // too pedantic.

      return tok::ERROR;
    }

    return tok::COMMENT_MULTI;
  } else {
    rewind();
    return tok::SLASH;
  }
}

//

auto Lexer::resetBuffer() -> void {
  std::memset(m_buffer, 0, sizeof(m_buffer));
}

auto Lexer::beginBuffer(const char c) -> void {
  resetBuffer();
  
  m_buffer[0] = c;
  m_bufferIndex = 1;
}

auto Lexer::continueBuffer(const char c) -> bool {
  if(m_bufferIndex == BUFFER_SIZE) {
    return NO;
  }

  m_buffer[m_bufferIndex] = c;
  ++m_bufferIndex;

  return YES;
}
} //namespace brutus
