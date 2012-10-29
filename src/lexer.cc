#include "lexer.h"

#define CONTINUE_BUFFER(x) \
  if(!continueBuffer(x)) { \
    return Token::kError; \
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
  "require",
  "pure",
  "immutable"
};

static const Token KeywordTokens[] = {
  Token::kThis,
  Token::kVal,
  Token::kVar,
  Token::kDef,
  Token::kFor,
  Token::kWhile,
  Token::kYield,
  Token::kTrue,
  Token::kFalse,
  Token::kYes,
  Token::kNo,
  Token::kLArrow,
  Token::kLArrow,
  Token::kRArrow,
  Token::kRArrow,
  Token::kIf,
  Token::kNew,
  Token::kOn,
  Token::kClass,
  Token::kTrait,
  Token::kVirtual,
  Token::kPublic,
  Token::kPrivate,
  Token::kProtected,
  Token::kInternal,
  Token::kNative,
  Token::kForce,
  Token::kModule,
  Token::kRequire,
  Token::kPure,
  Token::kImmutable
};

static const Token SingleCharacterTokens[] = {
  // 0-63
  Token::kError, //0
  Token::kError, //1
  Token::kError, //2
  Token::kError, //3
  Token::kError, //4
  Token::kError, //5
  Token::kError, //6
  Token::kError, //7
  Token::kError, //8
  Token::kError, //9
  Token::kError, //10
  Token::kError, //11
  Token::kError, //12
  Token::kError, //13
  Token::kError, //14
  Token::kError, //15
  Token::kError, //16
  Token::kError, //17
  Token::kError, //18
  Token::kError, //19
  Token::kError, //20
  Token::kError, //21
  Token::kError, //22
  Token::kError, //23
  Token::kError, //24
  Token::kError, //25
  Token::kError, //26
  Token::kError, //27
  Token::kError, //28
  Token::kError, //29
  Token::kError, //30
  Token::kError, //31
  Token::kError, //32
  Token::kError, //33
  Token::kError, //34
  Token::kHash, //35 '#'
  Token::kError, //36
  Token::kError, //37
  Token::kError, //38
  Token::kError, //39
  Token::kLParen, //40 '('
  Token::kRParen, //41 ')'
  Token::kError, //42 
  Token::kError, //43
  Token::kComma, //44 ','
  Token::kError, //45
  Token::kDot, //46 '.'
  Token::kError, //47
  Token::kError, //48
  Token::kError, //49
  Token::kError, //50
  Token::kError, //51
  Token::kError, //52
  Token::kError, //53
  Token::kError, //54
  Token::kError, //55
  Token::kError, //56
  Token::kError, //57
  Token::kColon, //58 ':'
  Token::kSemicolon, //59 ';'
  Token::kError, //60
  Token::kError, //61
  Token::kError, //62
  Token::kError, //63
  // 64-127
  Token::kError, //0
  Token::kError, //1
  Token::kError, //2
  Token::kError, //3
  Token::kError, //4
  Token::kError, //5
  Token::kError, //6
  Token::kError, //7
  Token::kError, //8
  Token::kError, //9
  Token::kError, //10
  Token::kError, //11
  Token::kError, //12
  Token::kError, //13
  Token::kError, //14
  Token::kError, //15
  Token::kError, //16
  Token::kError, //17
  Token::kError, //18
  Token::kError, //19
  Token::kError, //20
  Token::kError, //21
  Token::kError, //22
  Token::kError, //23
  Token::kError, //24
  Token::kError, //25
  Token::kError, //26
  Token::kLBrac, //27 '['
  Token::kError, //28
  Token::kRBrac, //29 ']'
  Token::kError, //30
  Token::kError, //31
  Token::kError, //32
  Token::kError, //33
  Token::kError, //34
  Token::kError, //35
  Token::kError, //36
  Token::kError, //37
  Token::kError, //38
  Token::kError, //39
  Token::kError, //40
  Token::kError, //41
  Token::kError, //42
  Token::kError, //43
  Token::kError, //44
  Token::kError, //45
  Token::kError, //46
  Token::kError, //47
  Token::kError, //48
  Token::kError, //49
  Token::kError, //50
  Token::kError, //51
  Token::kError, //52
  Token::kError, //53
  Token::kError, //54
  Token::kError, //55
  Token::kError, //56
  Token::kError, //57
  Token::kError, //58
  Token::kLBrace, //59 '{'
  Token::kError, //60
  Token::kRBrace, //61 '}'
  Token::kError, //62
  Token::kError//63
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
#define TOKEN_TO_STRING_CASE(T) case Token::##T: return ""#T
  switch(token) {
    TOKEN_TO_STRING_CASE(kEof);
    TOKEN_TO_STRING_CASE(kError);
    TOKEN_TO_STRING_CASE(kNewLine);
    TOKEN_TO_STRING_CASE(kWhitespace);
    TOKEN_TO_STRING_CASE(kIdentifier);
    TOKEN_TO_STRING_CASE(kNumberLiteral);
    TOKEN_TO_STRING_CASE(kStringLiteral);
    TOKEN_TO_STRING_CASE(kCommentSingle);
    TOKEN_TO_STRING_CASE(kCommentMulti);
    TOKEN_TO_STRING_CASE(kSemicolon);
    TOKEN_TO_STRING_CASE(kComma);
    TOKEN_TO_STRING_CASE(kLParen);
    TOKEN_TO_STRING_CASE(kRParen);
    TOKEN_TO_STRING_CASE(kLBrac);
    TOKEN_TO_STRING_CASE(kRBrac);
    TOKEN_TO_STRING_CASE(kLBrace);
    TOKEN_TO_STRING_CASE(kRBrace);
    TOKEN_TO_STRING_CASE(kDot);
    TOKEN_TO_STRING_CASE(kColon);
    TOKEN_TO_STRING_CASE(kAssign);
    TOKEN_TO_STRING_CASE(kLArrow);
    TOKEN_TO_STRING_CASE(kRArrow);
    TOKEN_TO_STRING_CASE(kHash);
    TOKEN_TO_STRING_CASE(kThis);
    TOKEN_TO_STRING_CASE(kVal);
    TOKEN_TO_STRING_CASE(kVar);
    TOKEN_TO_STRING_CASE(kDef);
    TOKEN_TO_STRING_CASE(kFor);
    TOKEN_TO_STRING_CASE(kWhile);
    TOKEN_TO_STRING_CASE(kYield);
    TOKEN_TO_STRING_CASE(kTrue);
    TOKEN_TO_STRING_CASE(kFalse);
    TOKEN_TO_STRING_CASE(kYes);
    TOKEN_TO_STRING_CASE(kNo);
    TOKEN_TO_STRING_CASE(kIf);
    TOKEN_TO_STRING_CASE(kNew);
    TOKEN_TO_STRING_CASE(kOn);
    TOKEN_TO_STRING_CASE(kClass);
    TOKEN_TO_STRING_CASE(kTrait);
    TOKEN_TO_STRING_CASE(kVirtual);
    TOKEN_TO_STRING_CASE(kPublic);
    TOKEN_TO_STRING_CASE(kPrivate);
    TOKEN_TO_STRING_CASE(kProtected);
    TOKEN_TO_STRING_CASE(kInternal);
    TOKEN_TO_STRING_CASE(kNative);
    TOKEN_TO_STRING_CASE(kForce);
    TOKEN_TO_STRING_CASE(kModule);
    TOKEN_TO_STRING_CASE(kRequire);
    TOKEN_TO_STRING_CASE(kPure);
    TOKEN_TO_STRING_CASE(kImmutable);
    default: return "<<unknown>>";
  }
  #undef TOKEN_TO_STRING_CASE
}

bool hasValue(const Token& token) {
  switch(token) {
    case Token::kIdentifier:
    case Token::kNumberLiteral:
    case Token::kStringLiteral:
    case Token::kCommentMulti:
    case Token::kCommentSingle:
      return YES;
    default:
      return NO;
  }
}
} //namespace tok

Lexer::Lexer() {
  init(nullptr);
}

void Lexer::init(CharStream* charStream)  {
  m_stream = charStream;
  m_line = 0;
  m_column = 0;
  m_currentChar = '\0';
  m_advanceWithLastChar = false;
  resetBuffer();
}

Token Lexer::resulting(
    std::function<bool(const char)> condition, //NOLINT
    std::function<bool(const char)> sideEffect, //NOLINT
    Token result) {
  while(canAdvance()) {
    const auto nextChar = advance();

    if(!condition(nextChar)) {
      rewind();
      break;
    } else {
      if(!sideEffect(nextChar)) {
        return Token::kError;
      }
    }
  }

  return result;
}

Token Lexer::nextToken() {
  while(canAdvance()) {
    auto currentChar = advance();

    if(isWhitespace(currentChar)) {
      // The current character represents whitespace. We consume
      // it until there is a character that is not considered
      // whitespace.

      return resulting(
        [&](const char c) { return isWhitespace(c); },
        [&](const char) { return YES; },
        Token::kWhitespace
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
        Token::kNewLine
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
        return Token::kAssign;
      }
    } else if('-' == currentChar) {
      if(advance() == '>') {
        // One could decide to allow operators like ->> but then again this 
        // becomes really confusing when looking at code like "x -> x ->> y" so 
        // the call is to disallow other operators that start with -> since this
        // should be a rare case anyways.
        return Token::kRArrow;
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
        return Token::kError;
      }
    }
  }

  return Token::kEof;
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

Token Lexer::continueWithNumberStart(const char currentChar) {
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
            return Token::kError;
          }
        } else if(isDigit(nextCharAfterE)) { 
          CONTINUE_BUFFER(nextCharAfterE);
        } else {
          // We have a literal of the form "1e" instead of "1e1".
          rewind();
          return Token::kError;
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

  return Token::kNumberLiteral;
}

Token Lexer::continueWithIdentifierStart(
    const char currentChar,
    bool operatorMode) {
  enum {
    kIdentifierOrOperator,
    kOperatorOnly,
    kIdentifierOnly
  } state;

  beginBuffer(currentChar);

  bool containsOperator = operatorMode;

  if(operatorMode) {
    state = kOperatorOnly;
  } else if(currentChar == '_') {
    state = kIdentifierOrOperator;
  } else {
    state = kIdentifierOnly;
  }

  while(canAdvance()) {
    const auto nextChar = advance();

    if(state == kIdentifierOrOperator) {
      if(isIdentifierPart(nextChar)) {
        state = nextChar == '_' ? kIdentifierOrOperator : kIdentifierOnly;
      } else if(isOperator(nextChar)) {
        state = kOperatorOnly;
        containsOperator = YES;
      } else {
        rewind();
        break;
      }
    } else if(state == kOperatorOnly) {
      if(!isOperator(nextChar)) {
        rewind();
        break;
      }
    } else if(state == kIdentifierOnly) {
      if(isIdentifierPart(nextChar)) {
        state = nextChar == '_' ? kIdentifierOrOperator : kIdentifierOnly;
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

  return Token::kIdentifier;
}

Token Lexer::continueWithBacktick() {
  const auto currentChar = advance();

  if(currentChar == '`') {
    // No empty name is allowed.
    return Token::kError;
  }

  beginBuffer(currentChar);

  while(canAdvance()) {
    const auto nextChar = advance();

    if(nextChar == '`') {
      // Reached the closing backtick so we have a valid
      // identifier that is not empty.
      return Token::kIdentifier;
    } else {
      // Found an arbitrary character we need to add to the buffer.
      CONTINUE_BUFFER(nextChar);
    }
  }

  // We reach this code only in case of a premature EOF.
  return Token::kError;
}

Token Lexer::continueWithString() {
  // We already have one " character.
  
  if(!canAdvance()) {
    // If we are at the EOF we have an open string
    // literal lotering around.
    return Token::kError;
  }

  const auto currentChar = advance();
  
  if('"' == currentChar) {
    // We have two " characters.

    if(!canAdvance()) {
      // If we reach the EOF at this point we have an empty string.
      return Token::kStringLiteral;
    }

    const auto possibleQuote = advance();

    if('"' == possibleQuote) {
      // We have three " characters. Go for a verbatim string literal.
      //TODO(joa): implement verbatim string literal
      return Token::kError;
    } else {
      // Empty string literal.
      rewind();
      resetBuffer();
      return Token::kStringLiteral;
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
          return Token::kStringLiteral;
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
                return Token::kError;
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
                return Token::kError;
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
                return Token::kError;
            }
          }

          isEscaped = NO;
        } else {
          CONTINUE_BUFFER(nextChar);
        }
      }
    }

    // Reached premature EOF while in a string literal.
    return Token::kError;
  }
}

Token Lexer::continueWithSlash(const char currentChar) {
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

    return Token::kCommentSingle;
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

      return Token::kError;
    }

    return Token::kCommentMulti;
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
