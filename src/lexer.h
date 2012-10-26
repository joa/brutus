#ifndef BRUTUS_LEXER_H_
#define BRUTUS_LEXER_H_

#include <cstring> // For std::memset and std::strcmp

#include "brutus.h"
#include "streams.h"

namespace brutus {
  namespace internal {
    enum class Token {
      kEof,
      kError,
      kNewLine,
      kWhitespace,

      kIdentifier,
      kNumberLiteral,
      kStringLiteral,

      kCommentSingle,
      kCommentMulti,

      // Control characters:
      kSemicolon,  // ;
      kComma,      // ,
      kLParen,     // (
      kRParen,     // )
      kLBrac,      // [
      kRBrac,      // ]
      kLBrace,     // {
      kRBrace,     // }
      kDot,        // .
      kColon,      // :
      kAssign,     // =
      kLArrow,     // <- and ←
      kRArrow,     // -> and →
      kHash,       // #

      // Keywords:
      kThis,
      kVal,
      kVar,
      kDef,
      kFor,
      kWhile,
      kYield,
      kTrue,
      kFalse,
      kYes,
      kNo,
      kIf,
      kNew,
      kOn,
      kClass,
      kTrait,
      kVirtual,
      kPublic,
      kPrivate,
      kProtected,
      kInternal,
      kNative,
      kForce,
      kModule,
      kRequire,
      kPure,
      kImmutable
    }; // enum Token

    namespace tok {
      const char* toString(const Token& token);
      bool hasValue(const Token& token);
    } //namespace tok
    
    class Lexer {
      public:
        explicit Lexer(CharStream* charStream);
        Token nextToken();
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

        Token continueWithNumberStart(const char currentChar);
        Token continueWithIdentifierStart(
            const char currentChar, bool operatorMode);
        Token continueWithSlash(const char currentChar);
        Token continueWithBacktick();
        Token continueWithString();

        Token resulting(
          std::function<bool(const char)> condition, //NOLINT
          std::function<bool(const char)> sideEffect, //NOLINT
          Token result);

        DISALLOW_COPY_AND_ASSIGN(Lexer);
    }; //class Lexer
  } //namespace internal
} //namespace brutus
#endif
