#ifndef _BRUTUS_H_
#define _BRUTUS_H_

#include <stdio.h>

#include <chrono>
#include <iostream>
#include <cstring>
#include <functional>

// Macro to disallow the invocation of copy constructor
// and assignment operator.
#ifdef __GNUC__
  #define DISALLOW_COPY_AND_ASSIGN(T) \
    T& operator=(const T&) = delete; \
    T&& operator=(const T&&) = delete; \
    T(const T&) = delete; \
    T(const T&&) = delete
#else
  // VS2012 does not support delete yet so we go with
  // the old version instead.
  #define DISALLOW_COPY_AND_ASSIGN(T) \
    T(const T&); \
    void operator=(const T&)
#endif

#if _MSC_VER
  // VS2012 does not support unicode literals yet so we
  // noop it for now.
  #define u8
#endif

#define YES true
#define NO false

#ifndef ALWAYS_INLINE
  #ifdef __GNUC__
    #define ALWAYS_INLINE __attribute__((always_inline))
  #else
    #define ALWAYS_INLINE inline
  #endif
#endif

#if _MSC_VER
  //VS2012 does not support constexpr yet so we fallback
  //to the _countof macro.
  #define NumberOfElements(x) _countof(x)
#else
  template<typename T, size_t sizeOfArray>
  constexpr size_t NumberOfElements(T (&)[sizeOfArray]) {
    return sizeOfArray;
  }
#endif

template<typename T>
ALWAYS_INLINE T* NewArray(const size_t& size) {
  auto result = new T[size];
  if(result == nullptr) {
    std::cerr << "Error: Could not allocate array.";
  }
  return result;
}

template<typename T>
ALWAYS_INLINE void DeleteArray(T* value) {
  delete[] value;
}

// Preconditions for compiling Brutus
static_assert(sizeof(char) == 1, "sizeof(char) must be one.");

namespace brutus {
  // Util
  class Stopwatch {
  public:
    explicit Stopwatch() {}
    void start();
    void stop();
    void log() const;
    void stopAndLog();

  private:
    DISALLOW_COPY_AND_ASSIGN(Stopwatch);
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
  }; // class Stopwatch

  // CharStream

  class CharStream {
  public:
    explicit CharStream() {}
    virtual ~CharStream() {}
    virtual bool hasNext() = 0;
    virtual char next() = 0;
    virtual void foreach(std::function<void(char)> f) = 0;

  private:
    DISALLOW_COPY_AND_ASSIGN(CharStream);
  }; // class CharStream

  class FileCharStream : public CharStream {
  public:
    explicit FileCharStream(FILE* file) : 
      m_file(file),
      m_bufferIndex(0),
      m_bufferLength(0) {}
    bool hasNext();
    char next();
    void foreach(std::function<void(char)> f);

  private:
    DISALLOW_COPY_AND_ASSIGN(FileCharStream);
    void updateBuffer();

    const FILE* m_file;
    char m_buffer[0x100];
    size_t m_bufferIndex;
    size_t m_bufferLength;
  }; // class FileCharStream

  // Lexer

  namespace tok {
  enum Token {
    _EOF,
    ERROR,
    NEWLINE,
    WHITESPACE,

    IDENTIFIER,

    NUMBER_LITERAL,
    STRING_LITERAL,

    COMMENT_SINGLE,
    COMMENT_MULTI,

    // Control characters:
    SEMICOLON,  //;
    COMMA,      //,
    LPAREN,     // (
    RPAREN,     // )
    LBRAC,      // [
    RBRAC,      // ]
    LBRACE,     // {
    RBRACE,     // }
    DOT,        // .
    COLON,      // :
    ASSIGN,     // =
    EQUALS,     // ==
    LARROW,     // <- and ←
    RARROW,     // -> and →

    // Keywords:
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
    IF
  }; // enum Token

  const char* toString(const Token& token);
  bool hasValue(const Token& token);
  } // namespace tok

  class Lexer {
  public:
    explicit Lexer(CharStream* charStream) : 
      m_stream(charStream),
      m_line(0),
      m_column(0),
      m_currentChar('\0'),
      m_advanceWithLastChar(false) {}

    tok::Token nextToken();
    const char* value();
    size_t valueLength();
    unsigned int posLine();
    unsigned int posColumn();

  private:
    DISALLOW_COPY_AND_ASSIGN(Lexer);

    static const size_t BUFFER_SIZE = 0x100;

    CharStream* const m_stream;
    unsigned int m_line, m_column;
    char m_currentChar;
    bool m_advanceWithLastChar;

    char m_buffer[BUFFER_SIZE];
    size_t m_bufferIndex;

    void resetBuffer();
    void beginBuffer(const char c);
    bool continueBuffer(const char c);

    bool canAdvance();
    char advance();
    void rewind();

    bool isWhitespace(const char c);
    bool isNewLine(const char c);
    bool isNumberStart(const char c);
    bool isDigit(const char c);

    bool isIdentifierStart(const char c);
    bool isObscureIdentifierStart(const char c);
    bool isIdentifierPart(const char c);

    tok::Token continueWithNumberStart(const char currentChar);
    tok::Token continueWithIdentifierStart(const char currentChar);
    tok::Token continueWithSlash(const char currentChar);

    tok::Token resulting(
      std::function<bool(const char)> condition,
      std::function<bool(const char)> sideEffect,
      tok::Token result);
  }; // class Lexer

  class Arena {
  public:
    explicit Arena() {}
  private:
    DISALLOW_COPY_AND_ASSIGN(Arena);
  }; // class Arena

  namespace ast {
    #include "ast.h"
  } //namespace ast

  class Parser {
  public:
    explicit Parser(Lexer* lexer) : m_lexer(lexer), m_arena() {}

    ast::Node* parseProgram();
    ast::Node* parseBlock();
    ast::Node* parseDefinition();
    void parseParameterList(ast::NodeList* list);
    ast::Node* parseParameter();
    ast::Node* parseExpression();
    ast::Node* parseExpression(bool allowInfixCall);
    ast::Node* parsePrimaryExpression();
    bool peekPrimaryExpression();
    ast::Node* continueWithExpression(ast::Node* expression, bool allowInfixCall);
    ast::Node* parseSelect(ast::Node* object);
    ast::Node* parseCall(ast::Node* callee);
    void parseArgumentList(ast::NodeList* list);
    ast::Node* parseArgument();
    ast::Node* parseSingleArgument();
    ast::Node* parseAnonymousFunctionExpression();
    void parseAnonymousFunctionParameterList(ast::NodeList* list);
    ast::Node* parseAnonymousFunctionParameter();
    ast::Node* parseThis();
    ast::Node* parseBooleanLiteral();
    ast::Node* parseNumberLiteral();
    ast::Node* parseStringLiteral();
    ast::Node* parseIfExpression();
    ast::Node* parseIfCase();
    ast::Node* parseVariableExpression();
    ast::Node* parseType();
    ast::Node* parseTypeParameterList(ast::NodeList* list);
    ast::Node* parseTypeParameter();
    ast::Node* parseIdentifier();
  private:
    DISALLOW_COPY_AND_ASSIGN(Parser);

    Lexer* const m_lexer;
    Arena m_arena;
    tok::Token m_currentToken;

    void advance();
    bool isIgnored(const tok::Token& token);
    bool peek(const tok::Token& token);
    bool poll(const tok::Token& token);
    void pollAll(const tok::Token& token);
    ast::Node* consume(const tok::Token& token, std::function<ast::Node*()> f);

    template<class T> T* alloc();
    template<class T> T* allocWithValue();

    ast::Node* error(const char* value);
  }; // class Parser
} // namespace brutus

#endif
