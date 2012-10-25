#ifndef BRUTUS_LEXER_H_
#define BRUTUS_LEXER_H_

#include <cstring> // For std::memset and std::strcmp

#include "brutus.h"
#include "streams.h"

namespace brutus {
  namespace internal {
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
        SEMICOLON,  // ;
        COMMA,      // ,
        LPAREN,     // (
        RPAREN,     // )
        LBRAC,      // [
        RBRAC,      // ]
        LBRACE,     // {
        RBRACE,     // }
        DOT,        // .
        COLON,      // :
        ASSIGN,     // =
        LARROW,     // <- and ←
        RARROW,     // -> and →
        HASH,       // #

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
      }; // enum Token

      const char* toString(const Token& token);
      bool hasValue(const Token& token);
    } //namespace tok
    
    class Lexer {
      public:
        explicit Lexer(CharStream* charStream);
        tok::Token nextToken();
        char* value();
        size_t valueLength();
        unsigned int posLine();
        unsigned int posColumn();

      private:
        static const size_t BUFFER_SIZE = 0x1000;

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
        bool isOperator(const char c);

        bool isIdentifierStart(const char c);
        bool isIdentifierPart(const char c);

        tok::Token continueWithNumberStart(const char currentChar);
        tok::Token continueWithIdentifierStart(
            const char currentChar, bool operatorMode);
        tok::Token continueWithSlash(const char currentChar);
        tok::Token continueWithBacktick();
        tok::Token continueWithString();

        tok::Token resulting(
          std::function<bool(const char)> condition, //NOLINT
          std::function<bool(const char)> sideEffect, //NOLINT
          tok::Token result);

        DISALLOW_COPY_AND_ASSIGN(Lexer);
    }; //class Lexer
  } //namespace internal
} //namespace brutus
#endif
