#ifndef _BRUTUS_H_
#define _BRUTUS_H_

#include <stdio.h>

#include <chrono>
#include <iostream>
#include <functional>

// Macro to disallow the invocation of copy constructor
// and assignment operator.
#define DISALLOW_COPY_AND_ASSIGN(T) \
  T& operator=(const T&) = delete; \
  T&& operator=(const T&&) = delete; \
  T(const T&) = delete; \
  T(const T&&) = delete

#define YES true
#define NO false

namespace brutus {
  // Util

  class Stopwatch {
  public:
    explicit Stopwatch() {}
    auto start() -> void;
    auto stop() -> void;
    auto log() -> void;
    auto stopAndLog() -> void;

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
    virtual auto hasNext() -> bool = 0;
    virtual auto next() -> char = 0;
    virtual auto foreach(std::function<void(char)> f) -> void = 0;

  private:
    DISALLOW_COPY_AND_ASSIGN(CharStream);
  }; // class CharStream

  class FileCharStream : public CharStream {
  public:
    explicit FileCharStream(FILE* file) : 
      m_file(file),
      m_bufferIndex(0),
      m_bufferLength(0) {}
    auto hasNext() -> bool;
    auto next() -> char;
    void foreach(std::function<void(char)> f);

  private:
    DISALLOW_COPY_AND_ASSIGN(FileCharStream);
    auto updateBuffer() -> void;

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
    BOOLEAN_LITERAL,
    STRING_LITERAL,

    COMMENT_SINGLE,
    COMMENT_MULTI,

    SEMICOLON,
    COMMA,
    LPAREN,
    RPAREN,
    LBRAC,
    RBRAC,
    LBRACE,
    RBRACE,
    DOT,
    COLON,

    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    PERCENT
  }; // enum Token

  auto toString(Token token) -> const char*;
  } // namespace tok

  class Lexer {
  public:
    explicit Lexer(CharStream* charStream) : 
      m_stream(charStream),
      m_line(0),
      m_column(0),
      m_currentChar('\0'),
      m_advanceWithLastChar(false) {}

    auto nextToken() -> tok::Token;
    auto value() -> const char*;
    auto posLine() -> int;
    auto posColumn() -> int;

  private:
    DISALLOW_COPY_AND_ASSIGN(Lexer);
    CharStream* const m_stream;
    int m_line, m_column;
    char m_currentChar;
    bool m_advanceWithLastChar;

    auto canAdvance() -> bool;
    auto advance() -> char;
    auto rewind() -> void;

    auto isWhitespace(const char c) -> bool;
    auto isNewLine(const char c) -> bool;
    auto isNumberStart(const char c) -> bool;
    auto isDigit(const char c) -> bool;

    auto isIdentifierStart(const char c) -> bool;
    auto isIdentifierPart(const char c) -> bool;

    auto continueWithNumberStart(const char currentChar) -> tok::Token;
    auto continueWithIdentifierStart(const char currentChar) -> tok::Token;
    auto continueWithSlash(const char currentChar) -> tok::Token;

    auto resulting(
      std::function<bool(const char)> condition,
      std::function<void(const char)> f,
      tok::Token result) -> tok::Token;
  }; // class Lexer
} // namespace brutus

#endif
