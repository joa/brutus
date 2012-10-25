#include "lexer.h"

#define CONTINUE_BUFFER(x) \
  if(!continueBuffer(x)) { \
    return tok::ERROR; \
  }

namespace brutus {
namespace internal {
namespace tok {
static const char* KeywordChars[] = {
  "this",
  "val",
  "var",
  "def",
  "for",
  "while",
  "yield",
  "true",
  "false",
  "yes",
  "no",
  "<-",
  u8"←",
  "->",
  u8"→",
  "if",
  "new",
  "on",
  "class",
  "trait",
  "virtual",
  "public",
  "private",
  "protected",
  "internal",
  "native",
  "force",
  "module",
  "require"
};

static const Token KeywordTokens[] = {
  THIS,
  VAL,
  VAR,
  DEF,
  FOR,
  WHILE,
  YIELD,
  TRUE_,
  FALSE_,
  YES_,
  NO_,
  LARROW,
  LARROW,
  RARROW,
  RARROW,
  IF,
  NEW,
  ON,
  CLASS,
  TRAIT,
  VIRTUAL,
  PUBLIC,
  PRIVATE,
  PROTECTED,
  INTERNAL,
  NATIVE,
  FORCE,
  MODULE,
  REQUIRE
};

static const Token SingleCharacterTokens[] = {
  // 0-63
  ERROR, //0
  ERROR, //1
  ERROR, //2
  ERROR, //3
  ERROR, //4
  ERROR, //5
  ERROR, //6
  ERROR, //7
  ERROR, //8
  ERROR, //9
  ERROR, //10
  ERROR, //11
  ERROR, //12
  ERROR, //13
  ERROR, //14
  ERROR, //15
  ERROR, //16
  ERROR, //17
  ERROR, //18
  ERROR, //19
  ERROR, //20
  ERROR, //21
  ERROR, //22
  ERROR, //23
  ERROR, //24
  ERROR, //25
  ERROR, //26
  ERROR, //27
  ERROR, //28
  ERROR, //29
  ERROR, //30
  ERROR, //31
  ERROR, //32
  ERROR, //33
  ERROR, //34
  HASH, //35 '#'
  ERROR, //36
  ERROR, //37
  ERROR, //38
  ERROR, //39
  LPAREN, //40 '('
  RPAREN, //41 ')'
  ERROR, //42 
  ERROR, //43
  COMMA, //44 ','
  ERROR, //45
  DOT, //46 '.'
  ERROR, //47
  ERROR, //48
  ERROR, //49
  ERROR, //50
  ERROR, //51
  ERROR, //52
  ERROR, //53
  ERROR, //54
  ERROR, //55
  ERROR, //56
  ERROR, //57
  COLON, //58 ':'
  SEMICOLON, //59 ';'
  ERROR, //60
  ERROR, //61
  ERROR, //62
  ERROR, //63
  // 64-127
  ERROR, //0
  ERROR, //1
  ERROR, //2
  ERROR, //3
  ERROR, //4
  ERROR, //5
  ERROR, //6
  ERROR, //7
  ERROR, //8
  ERROR, //9
  ERROR, //10
  ERROR, //11
  ERROR, //12
  ERROR, //13
  ERROR, //14
  ERROR, //15
  ERROR, //16
  ERROR, //17
  ERROR, //18
  ERROR, //19
  ERROR, //20
  ERROR, //21
  ERROR, //22
  ERROR, //23
  ERROR, //24
  ERROR, //25
  ERROR, //26
  LBRAC, //27 '['
  ERROR, //28
  RBRAC, //29 ']'
  ERROR, //30
  ERROR, //31
  ERROR, //32
  ERROR, //33
  ERROR, //34
  ERROR, //35
  ERROR, //36
  ERROR, //37
  ERROR, //38
  ERROR, //39
  ERROR, //40
  ERROR, //41
  ERROR, //42
  ERROR, //43
  ERROR, //44
  ERROR, //45
  ERROR, //46
  ERROR, //47
  ERROR, //48
  ERROR, //49
  ERROR, //50
  ERROR, //51
  ERROR, //52
  ERROR, //53
  ERROR, //54
  ERROR, //55
  ERROR, //56
  ERROR, //57
  ERROR, //58
  LBRACE, //59 '{'
  ERROR, //60
  RBRACE, //61 '}'
  ERROR, //62
  ERROR //63
};

static_assert(
  NumberOfElements(SingleCharacterTokens) == 0x80,
  "Single character token table must have a size of 0x80.");
static_assert(';' == 59, "';' must equal 59.");
static_assert(',' == 44, "',' must equal 44.");
static_assert('(' == 40, "'(' must equal 40.");
static_assert(')' == 41, "')' must equal 41.");
static_assert('[' == 91, "'[' must equal 91.");
static_assert(']' == 93, "']' must equal 93.");
static_assert('{' == 123, "'{' must equal 123.");
static_assert('}' == 125, "'}' must equal 125.");
static_assert('.' == 46, "'.' must equal 46.");
static_assert(':' == 58, "':' must equal 58.");
static_assert('#' == 35, "'#' must equal 35.");

static const size_t NUM_KEYWORDS = NumberOfElements(KeywordChars);

const char* toString(const Token& token) {
  #define TOKEN_TO_STRING_CASE(T, S) case T: return S
  switch(token) {
    TOKEN_TO_STRING_CASE(_EOF, "EOF");
    TOKEN_TO_STRING_CASE(ERROR, "ERROR");
    TOKEN_TO_STRING_CASE(NEWLINE, "NEWLINE");
    TOKEN_TO_STRING_CASE(WHITESPACE, "WHITESPACE");
    TOKEN_TO_STRING_CASE(IDENTIFIER, "IDENTIFIER");
    TOKEN_TO_STRING_CASE(NUMBER_LITERAL, "NUMBER_LITERAL");
    TOKEN_TO_STRING_CASE(STRING_LITERAL, "STRING_LITERAL");
    TOKEN_TO_STRING_CASE(COMMENT_MULTI, "COMMENT_MULTI");
    TOKEN_TO_STRING_CASE(COMMENT_SINGLE, "COMMENT_SINGLE");
    TOKEN_TO_STRING_CASE(SEMICOLON, "SEMICOLON");
    TOKEN_TO_STRING_CASE(COMMA, "COMMA");
    TOKEN_TO_STRING_CASE(LPAREN, "LPAREN");
    TOKEN_TO_STRING_CASE(RPAREN, "RPAREN");
    TOKEN_TO_STRING_CASE(LBRAC, "LBRAC");
    TOKEN_TO_STRING_CASE(RBRAC, "RBRAC");
    TOKEN_TO_STRING_CASE(LBRACE, "LBRACE");
    TOKEN_TO_STRING_CASE(RBRACE, "RBRACE");
    TOKEN_TO_STRING_CASE(DOT, "DOT");
    TOKEN_TO_STRING_CASE(COLON, "COLON");
    TOKEN_TO_STRING_CASE(ASSIGN, "ASSIGN");
    TOKEN_TO_STRING_CASE(HASH, "HASH");
    TOKEN_TO_STRING_CASE(THIS, "THIS");
    TOKEN_TO_STRING_CASE(VAL, "VAL");
    TOKEN_TO_STRING_CASE(VAR, "VAR");
    TOKEN_TO_STRING_CASE(DEF, "DEF");
    TOKEN_TO_STRING_CASE(FOR, "FOR");
    TOKEN_TO_STRING_CASE(WHILE, "WHILE");
    TOKEN_TO_STRING_CASE(YIELD, "YIELD");
    TOKEN_TO_STRING_CASE(TRUE_, "TRUE");
    TOKEN_TO_STRING_CASE(FALSE_, "FALSE");
    TOKEN_TO_STRING_CASE(YES_, "YES");
    TOKEN_TO_STRING_CASE(NO_, "NO");
    TOKEN_TO_STRING_CASE(IF, "IF");
    TOKEN_TO_STRING_CASE(NEW, "NEW");
    TOKEN_TO_STRING_CASE(ON, "ON");
    TOKEN_TO_STRING_CASE(CLASS, "CLASS");
    TOKEN_TO_STRING_CASE(TRAIT, "TRAIT");
    TOKEN_TO_STRING_CASE(VIRTUAL, "VIRTUAL");
    TOKEN_TO_STRING_CASE(PUBLIC, "PUBLIC");
    TOKEN_TO_STRING_CASE(PRIVATE, "PRIVATE");
    TOKEN_TO_STRING_CASE(PROTECTED, "PROTECTED");
    TOKEN_TO_STRING_CASE(INTERNAL, "INTERNAL");
    TOKEN_TO_STRING_CASE(NATIVE, "NATIVE");
    TOKEN_TO_STRING_CASE(FORCE, "FORCE");
    TOKEN_TO_STRING_CASE(MODULE, "MODULE");
    TOKEN_TO_STRING_CASE(REQUIRE, "REQUIRE");
    default: return "UNKNOWN";
  }
  #undef TOKEN_TO_STRING_CASE
}

bool hasValue(const Token& token) {
  switch(token) {
    case IDENTIFIER:
    case NUMBER_LITERAL:
    case STRING_LITERAL:
    case COMMENT_MULTI:
    case COMMENT_SINGLE:
      return YES;
    default:
      return NO;
  }
}
} //namespace tok

Lexer::Lexer(CharStream* charStream)
    : m_stream(charStream),
      m_line(0),
      m_column(0),
      m_currentChar('\0'),
      m_advanceWithLastChar(false) {}

tok::Token Lexer::resulting(
    std::function<bool(const char)> condition, //NOLINT
    std::function<bool(const char)> sideEffect, //NOLINT
    tok::Token result) {
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

tok::Token Lexer::nextToken() {
  while(canAdvance()) {
    auto currentChar = advance();

    if(isWhitespace(currentChar)) {
      // The current character represents whitespace. We consume
      // it until there is a character that is not considered
      // whitespace.

      return resulting(
        [&](const char c) { return isWhitespace(c); },
        [&](const char) { return YES; },
        tok::WHITESPACE
      );
    } else if(isNewLine(currentChar)) {
      // When a new line character is encountered we will simply
      // update the position information and consume until there
      // is a character that is not considered a new line.
      //
      // This also means that '\n' is considered equal to "\n\n"
      // and allows people to use more new lines if they want to.
      //
      // It is important to note the side effect which resets the
      // position information.
      ++m_line;
      m_column = 0;
      
      return resulting(
        [&](const char c) { return isNewLine(c); },
        [&](const char) -> bool {
          ++m_line;
          m_column = 0;

          return YES;
        },
        tok::NEWLINE
      );
    } else if(isNumberStart(currentChar)) {
      return continueWithNumberStart(currentChar);
    } else if('"' == currentChar) {
      return continueWithString();
    } else if(isIdentifierStart(currentChar)) {
      return continueWithIdentifierStart(currentChar, /*operatorMode=*/NO);
    } else if('=' == currentChar) {
      if(isOperator(advance())) {
        rewind();
        return continueWithIdentifierStart(currentChar, /*operatorMode=*/YES);
      } else {
        rewind();
        return tok::ASSIGN;
      }
    } else if('-' == currentChar) {
      if(advance() == '>') {
        // One could decide to allow operators like ->> but then again this 
        // becomes really confusing when looking at code like "x -> x ->> y" so 
        // the call is to disallow other operators that start with -> since this
        // should be a rare case anyways.
        return tok::RARROW;
      } else {
        rewind();
        return continueWithIdentifierStart(currentChar, /*operatorMode=*/YES);
      }
    } else if('/' == currentChar) {
      return continueWithSlash(currentChar);
    } else if('`' == currentChar) {
      // Parse until `. No escaping allowed. The resulting identifier
      // will contain the name sans ` characters.
      return continueWithBacktick();
    } else if(isOperator(currentChar)) {
      // The generic operator handling comes after '=', '-' and '/'
      return continueWithIdentifierStart(currentChar, /*operatorMode=*/YES);
    } else {
      auto charCode = static_cast<int>(currentChar);

      if(charCode >= 0x00 && charCode <= 0x80) {
        return tok::SingleCharacterTokens[charCode];
      } else {
        return tok::ERROR;
      }
    }
  }

  return tok::_EOF;
}

char* Lexer::value() {
  return m_buffer;
}

size_t Lexer::valueLength() {
  return m_bufferIndex + 1;
}

unsigned int Lexer::posLine() {
  return m_line;
}

unsigned int Lexer::posColumn() {
  return m_column;
}

bool Lexer::canAdvance() {
  return m_advanceWithLastChar || m_stream->hasNext();
}

char Lexer::advance() {
  if(!canAdvance()) {
    return '\0';
  }

  if(m_advanceWithLastChar) {
    m_advanceWithLastChar = NO;
  } else {
    m_currentChar = m_stream->next();
  }

  ++m_column;
  
  return m_currentChar;
}

void Lexer::rewind() {
#ifdef DEBUG
  if(m_advanceWithLastChar) {
    std::cout << "Error: Called Lexer::rewind() twice in a row." << std::endl;
  }
#endif

  m_advanceWithLastChar = YES;
  --m_column;
}

bool Lexer::isWhitespace(const char c) {
  // Note that we explicitly forbid \t in Brutus
  // source code so only space characters are treated
  // as whitespace.
  //
  // For completeness we should also check for the unicode 
  // characters however they wont be matched here anyways.
  //
  // Instead we will have to do this once unicode
  // character support has settled in the lexer.
  return c == ' ' || c == '\xA0';// || c == '\x2007' || c == '\x202f';
}

bool Lexer::isNewLine(const char c) {
  // Note that we explicitly forbid \r in Brutus
  // source code so only \n is allowed as a line
  // terminator.
  return c == '\n';
}

bool Lexer::isNumberStart(const char c) {
  return c >= '0' && c <= '9';
}

//TODO(joa): fix unicode handling and accept correct chars.

bool Lexer::isIdentifierStart(const char c) {
  return (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z')
      || ((unsigned char)c >= 0xC0)
      || c == '_';
}

bool Lexer::isIdentifierPart(const char c) {
  return isIdentifierStart(c)
      || (c >= '0' && c <= '9')
      || ((unsigned char)c >= 0x80);
}

bool Lexer::isDigit(const char c) {
  return (c >= '0' && c <= '9');
}

bool Lexer::isOperator(const char c) {
  return c == '!'
      || c == '$'
      || c == '%'
      || c == '&'
      || c == '*'
      || c == '+'
      || c == '-'
      || c == '/'
      || c == '<'
      || c == '>'
      || c == '?'
      || c == '\\'
      || c == '^'
      || c == '|'
      || c == '~'
      || c == '=';
}

//

tok::Token Lexer::continueWithNumberStart(const char currentChar) {
  beginBuffer(currentChar);

  // We allow only 0xffffff not 0XFFFFFF, not 0xFFFFFF
  const auto secondChar = advance();

  if(secondChar == 'x') {
    // Parse hexadecimal literal
    
    CONTINUE_BUFFER(secondChar);
    
    while(canAdvance()) {
      const auto hexChar = advance();

      if(isDigit(hexChar) || (hexChar >= 'a' && hexChar <= 'f')) {
        CONTINUE_BUFFER(hexChar);
      } else {
        rewind();
        break;
      }
    }
  } else if(isDigit(secondChar) || secondChar == 'e' || secondChar == '.') {
    CONTINUE_BUFFER(secondChar);

    bool eAllowed = secondChar != 'e';
    bool dotAllowed = eAllowed && secondChar != '.';
    
    // Parse a digit of one of the following forms:
    //
    // 12 ...
    // 1e ...
    // 1. ...

    while(canAdvance()) {
      const auto nextChar = advance();

      if(isDigit(nextChar)) {
        CONTINUE_BUFFER(nextChar);
      } else if(eAllowed && nextChar == 'e') {
        eAllowed = NO;
        dotAllowed = NO;
        
        CONTINUE_BUFFER(nextChar);

        const auto nextCharAfterE = advance();

        if(nextCharAfterE == '-') {
          // Special treatment for the '-' character:
          // It is allowed immediately after the 'e'
          // character but nowhere else.
          //
          // If we find a '-' character we need at least
          // another digit or the literal is illegal.

          CONTINUE_BUFFER(nextCharAfterE);

          const auto nextCharAfterMinus = advance();

          if(isDigit(nextCharAfterMinus)) {
            CONTINUE_BUFFER(nextCharAfterMinus);
          } else {
            // This is an illegal number literal since we
            // have something of the form "1e-" instead
            // of "1e-1".
            rewind();
            return tok::ERROR;
          }
        } else if(isDigit(nextCharAfterE)) { 
          CONTINUE_BUFFER(nextCharAfterE);
        } else {
          // We have a literal of the form "1e" instead of "1e1".
          rewind();
          return tok::ERROR;
        }
      } else if(dotAllowed && nextChar == '.') {
        dotAllowed = false;
        CONTINUE_BUFFER(nextChar);
      } else {
        rewind();
        break;
      }
    }
  } else {
    // This was only a single digit.
    rewind();
  }

  // We can also do something like this to 
  // check for a long value.
  //
  // if(advance() == 'L') {
  //
  // }

  return tok::NUMBER_LITERAL;
}

tok::Token Lexer::continueWithIdentifierStart(
    const char currentChar,
    bool operatorMode) {
  enum {
    IDENTIFIER_OR_OPERATOR,
    OPERATOR_ONLY,
    IDENTIFIER_ONLY
  } state;

  beginBuffer(currentChar);

  bool containsOperator = operatorMode;

  if(operatorMode) {
    state = OPERATOR_ONLY;
  } else if(currentChar == '_') {
    state = IDENTIFIER_OR_OPERATOR;
  } else {
    state = IDENTIFIER_ONLY;
  }

  while(canAdvance()) {
    const auto nextChar = advance();

    if(state == IDENTIFIER_OR_OPERATOR) {
      if(isIdentifierPart(nextChar)) {
        state = nextChar == '_' ? IDENTIFIER_OR_OPERATOR : IDENTIFIER_ONLY;
      } else if(isOperator(nextChar)) {
        state = OPERATOR_ONLY;
        containsOperator = YES;
      } else {
        rewind();
        break;
      }
    } else if(state == OPERATOR_ONLY) {
      if(!isOperator(nextChar)) {
        rewind();
        break;
      }
    } else if(state == IDENTIFIER_ONLY) {
      if(isIdentifierPart(nextChar)) {
        state = nextChar == '_' ? IDENTIFIER_OR_OPERATOR : IDENTIFIER_ONLY;
      } else {
        rewind();
        break;
      }
    }

    CONTINUE_BUFFER(nextChar);
  }

  //TODO(joa): optimize keyword lookup
  if(!containsOperator) {
    // No keyword contains an operator so we can skip the 
    // check if the identifier contains an operator.
    for(size_t i = 0; i < tok::NUM_KEYWORDS; ++i) {
      if(0 == std::strcmp(tok::KeywordChars[i], m_buffer)) {
        return tok::KeywordTokens[i];
      }
    }
  }

  return tok::IDENTIFIER;
}

tok::Token Lexer::continueWithBacktick() {
  const auto currentChar = advance();

  if(currentChar == '`') {
    // No empty name is allowed.
    return tok::ERROR;
  }

  beginBuffer(currentChar);

  while(canAdvance()) {
    const auto nextChar = advance();

    if(nextChar == '`') {
      // Reached the closing backtick so we have a valid
      // identifier that is not empty.
      return tok::IDENTIFIER;
    } else {
      // Found an arbitrary character we need to add to the buffer.
      CONTINUE_BUFFER(nextChar);
    }
  }

  // We reach this code only in case of a premature EOF.
  return tok::ERROR;
}

tok::Token Lexer::continueWithString() {
  // We already have one " character.
  
  if(!canAdvance()) {
    // If we are at the EOF we have an open string
    // literal lotering around.
    return tok::ERROR;
  }

  const auto currentChar = advance();
  
  if('"' == currentChar) {
    // We have two " characters.

    if(!canAdvance()) {
      // If we reach the EOF at this point we have an empty string.
      return tok::STRING_LITERAL;
    }

    const auto possibleQuote = advance();

    if('"' == possibleQuote) {
      // We have three " characters. Go for a verbatim string literal.
      //TODO(joa): implement verbatim string literal
      return tok::ERROR;
    } else {
      // Empty string literal.
      rewind();
      resetBuffer();
      return tok::STRING_LITERAL;
    }
  } else {
    beginBuffer(currentChar);

    bool isEscaped = NO;

    while(canAdvance()) {
      const auto nextChar = advance();

      if('"' == nextChar) {
        if(isEscaped) {
          CONTINUE_BUFFER(nextChar);
          isEscaped = NO;
        } else {
          return tok::STRING_LITERAL;
        }
      } else if('\\' == nextChar) {
        if(isEscaped) {
          CONTINUE_BUFFER(nextChar);
          isEscaped = NO;
        } else {
          isEscaped = YES;
        }
      } else {
        if(isEscaped) {
          if(isDigit(nextChar)) {
            char c = nextChar;

            int i = 0;
            int code = 0;

            do {
              int value = c - '0';

              if(value > 7) {
                // A value greater than 7 is not valid in
                // octal notation
                return tok::ERROR;
              }

              code = (code << 3) + value;

              if(canAdvance()) {
                c = advance();

                if(!isDigit(c)) {
                  rewind();
                  break;
                }
              } else {
                // String literal is not closed, got EOF
                return tok::ERROR;
              }
            } while(++i < 3);

            CONTINUE_BUFFER(static_cast<char>(code));
          } else {
            switch(nextChar) {
              case '$': CONTINUE_BUFFER('$'); break;
              case 'a': CONTINUE_BUFFER('\a'); break;
              case 'b': CONTINUE_BUFFER('\b'); break;
              case 'f': CONTINUE_BUFFER('\f'); break;
              case 'n': CONTINUE_BUFFER('\n'); break;
              case 'r': CONTINUE_BUFFER('\r'); break;
              case 't': CONTINUE_BUFFER('\t'); break;
              case 'v': CONTINUE_BUFFER('\v'); break;
              case 'u':
                //TODO(joa): implement \uXXXX ...
                break;
              default:
                // Illegal escape sequence.
                return tok::ERROR;
            }
          }

          isEscaped = NO;
        } else {
          CONTINUE_BUFFER(nextChar);
        }
      }
    }

    // Reached premature EOF while in a string literal.
    return tok::ERROR;
  }
}

tok::Token Lexer::continueWithSlash(const char currentChar) {
  beginBuffer(currentChar);

  const auto nextChar = advance(); 

  if(nextChar == '/') {
    CONTINUE_BUFFER(nextChar);

    while(canAdvance()) {
      char singleLineChar = advance();

      if(isNewLine(singleLineChar)) {
        break;
      }

      CONTINUE_BUFFER(singleLineChar);
    }

    return tok::COMMENT_SINGLE;
  } else if(nextChar == '*') {
    CONTINUE_BUFFER(nextChar);

    int openComments = 1;
    
    while(openComments > 0 && canAdvance()) {
      const auto commentChar = advance();

      CONTINUE_BUFFER(commentChar);

      if(commentChar == '/') {
        if(advance() == '*') {
          CONTINUE_BUFFER('*');
          ++openComments;
        } else {
          rewind();
        }
      } else if(commentChar == '*') {
        if(advance() == '/') {
          CONTINUE_BUFFER('/');
          --openComments;
        } else {
          rewind();
        }
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
    return continueWithIdentifierStart(currentChar, /*operatorMode=*/YES);
  }
}

//

void Lexer::resetBuffer() {
  std::memset(m_buffer, 0, sizeof(m_buffer));
}

void Lexer::beginBuffer(const char c) {
  resetBuffer();
  
  m_buffer[0] = c;
  m_bufferIndex = 1;
}

bool Lexer::continueBuffer(const char c) {
  if(m_bufferIndex == BUFFER_SIZE) {
    return NO;
  }

  m_buffer[m_bufferIndex] = c;
  ++m_bufferIndex;

  return YES;
}
} //namespace internal
} //namespace brutus
