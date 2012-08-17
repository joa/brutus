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
  T(const T&) = delete

namespace brutus {
  // Util

  class Stopwatch {
    public:
    explicit Stopwatch() {}
    void start();
    void stop();
    void log();
    void stopAndLog();

    private:
    DISALLOW_COPY_AND_ASSIGN(Stopwatch);
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
  };

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
  };

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
  };

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
  }; 

  const char* toString(Token token);
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
    int posLine();
    int posColumn();

    private:
    DISALLOW_COPY_AND_ASSIGN(Lexer);
    CharStream* const m_stream;
    int m_line, m_column;
    char m_currentChar;
    bool m_advanceWithLastChar;

    bool canAdvance();
    char advance();
    void rewind();

    bool isWhitespace(const char c);
    bool isNewLine(const char c);

    bool isIdentifierStart(const char c);
    bool isIdentifierPart(const char c);

    bool isNumberStart(const char c);
    bool isNumberPart(const char c);

    tok::Token resulting(
      std::function<bool(const char)> condition,
      std::function<void(const char)> f,
      tok::Token result);
  };
}

#endif
