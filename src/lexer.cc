#include "brutus.h"

namespace brutus {
namespace tok {
const char* toString(Token token) {
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
}

tok::Token Lexer::resulting(
    std::function<bool(const char)> condition,
    std::function<void(const char)> f,
    tok::Token result) {
  while(canAdvance()) {
    const char nextChar = advance();

    if(!condition(nextChar)) {
      rewind();
      break;
    } else {
      f(nextChar);
    }
  }

  return result;
}

tok::Token Lexer::nextToken() {
  while(canAdvance()) {
    auto currentChar = advance();

    if(isWhitespace(currentChar)) {
      return resulting(
        [&](const char c) { return isWhitespace(c); },
        [&](const char) { /* empty */ },
        tok::WHITESPACE
      );
    } else if(isNewLine(currentChar)) {
      ++m_line;

      return resulting(
        [&](const char c) { return isNewLine(c); },
        [&](const char) { ++m_line; },
        tok::NEWLINE
      );
    } else if(isNumberStart(currentChar)) {
      //reset buffer
      //add currentChar to buffer
      
      return resulting(
        [&](const char c) { return isNumberPart(c); },
        [&](const char) { /* add char to buffer */ },
        tok::NUMBER_LITERAL
      );
    } else if(isIdentifierStart(currentChar)) {
      // reset buffer
      // add currentChar to buffer

      return resulting(
        [&](const char c) { return isIdentifierPart(c); },
        [&](const char) { /* add char to buffer */ },
        tok::IDENTIFIER
      );
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
        case '/': {
          const char nextChar = advance(); 

          if(nextChar == '/') {
            // single line comment
            return tok::ERROR;
          } else if(nextChar == '*') {
            // multi line comment (do not forget nesting)
            return tok::ERROR;
          } else {
            rewind();
            return tok::SLASH;
          }
        }
        case '%': return tok::PERCENT;

        default:
          return tok::ERROR;
      }
    }
  }

  return tok::_EOF;
}

const char* Lexer::value() {
  return u8"";
}

int Lexer::posLine() {
  return m_line;
}

int Lexer::posColumn() {
  return m_column;
}

bool Lexer::canAdvance() {
  return m_advanceWithLastChar || m_stream->hasNext();
}

char Lexer::advance() {
  if(m_advanceWithLastChar) {
    m_advanceWithLastChar = false;
  } else {
    m_currentChar = m_stream->next();
  }

  ++m_column;
  
  return m_currentChar;
}

void Lexer::rewind() {
#ifdef DEBUG
  if(m_advanceWithLastChar) {
    std::cout << u8"Error: Called Lexer::rewind() twice in a row." << std::endl;
  }
#endif

  m_advanceWithLastChar = true;
  --m_column;
}

bool Lexer::isWhitespace(const char c) {
  // Note that we explicitly forbid \t in Brutus
  // source code so only space characters are treated
  // as whitespace.
  return c == ' ';
}

bool Lexer::isNewLine(const char c) {
  // Note that we explicitly forbid \r in Brutus
  // source code so only \n is allowed as a line
  // terminator.
  return c == '\n';
}

bool Lexer::isIdentifierStart(const char c) {
  return (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z')
      || (c == '_');
}

bool Lexer::isIdentifierPart(const char c) {
  return isIdentifierStart(c)
      || (c >= '0' && c <= '9');
}

bool Lexer::isNumberStart(const char c) {
  return (c >= '0' && c <= '9');
}

bool Lexer::isNumberPart(const char c) {
  return isNumberStart(c);
}
}
