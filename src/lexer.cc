#include "lexer.h"

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
  "native"
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
  NATIVE
};

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
  ERROR, //63
};

static_assert(NumberOfElements(SingleCharacterTokens) == 0x80, "Single character token table must have a size of 0x80.");

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
    TOKEN_TO_STRING_CASE(EQUALS, "EQUALS");
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
    default: return "UNKNOWN";
  }
  #undef TOKEN_TO_STRING_CASE
}

bool hasValue(const Token& token) {
  switch(token) {
    case IDENTIFIER:
    case NUMBER_LITERAL:
    case STRING_LITERAL:
      return YES;
    default:
      return NO;
  }
}
} //namespace tok

tok::Token Lexer::resulting(
    std::function<bool(const char)> condition,
    std::function<bool(const char)> sideEffect,
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

//TODO(joa): the lexer is currently not smart enough. and a decision has to be made. is "a=123" a valid identifier? it shouldn't be...

tok::Token Lexer::nextToken() {
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
      m_column = 0;
      
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
        case '-': 
          if(advance() == '>') {
            return tok::RARROW;
          } else {
            rewind();
            return continueWithIdentifierStart(currentChar);
          }
        case '=': 
          if(advance() == '=') {
            return tok::EQUALS;
          } else {
            rewind();
            return tok::ASSIGN;
          }

        default:
          if(currentChar >= 0x00 && currentChar < 0x80) {
            return tok::SingleCharacterTokens[currentChar];
          }

          return tok::ERROR;
      }
    }
  }

  return tok::_EOF;
}

const char* Lexer::value() {
  return m_buffer;
}

size_t Lexer::valueLength() {
  return m_bufferIndex;
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
  return c == ' ';
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
      || isObscureIdentifierStart(c);
}

//TODO(joa): isObscureIdentifierStart will become isOperatorStart
bool Lexer::isObscureIdentifierStart(const char c) {
  return c == '!'
      || c == '#'
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
      || c == '_'
      || c == '|'
      || c == '~';
}
bool Lexer::isIdentifierPart(const char c) {
  return isIdentifierStart(c)
      || (c >= '0' && c <= '9')
      || ((unsigned char)c >= 0x80);
}

bool Lexer::isDigit(const char c) {
  return (c >= '0' && c <= '9');
}

//

tok::Token Lexer::continueWithNumberStart(const char currentChar) {
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

tok::Token Lexer::continueWithIdentifierStart(const char currentChar) {
  beginBuffer(currentChar);

  auto ident = resulting(
    [&](const char c) { return isIdentifierPart(c); },
    [&](const char c) { return continueBuffer(c); },
    tok::IDENTIFIER
  );

  if(ident == tok::IDENTIFIER) {
    // If we really got an identifier we will check for a keyword.

    //TODO(joa): optimize keyword lookup
    for(size_t i = 0; i < tok::NUM_KEYWORDS; ++i) {
      if(0 == std::strcmp(tok::KeywordChars[i], m_buffer)) {
        return tok::KeywordTokens[i];
      }
    }
  }

  return ident;
}

tok::Token Lexer::continueWithSlash(const char currentChar) {
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
    return continueWithIdentifierStart(currentChar);
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
