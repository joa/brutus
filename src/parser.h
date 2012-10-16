#ifndef BRUTUS_PARSER_H_
#define BRUTUS_PARSER_H_

#include "brutus.h"
#include "arena.h"
#include "ast.h"
#include "lexer.h"

namespace brutus {
  namespace internal {
    class Parser {
      public:
        static const int ACC_VIRTUAL = 1;
        static const int ACC_PUBLIC = 1 << 1;
        static const int ACC_PRIVATE = 1 << 2;
        static const int ACC_PROTECTED = 1 << 3;
        static const int ACC_INTERNAL = 1 << 4;
        static const int ACC_NATIVE = 1 << 5;
        static const int ACC_FORCE = 1 << 6;
        static const int ACC_ABSTRACT = 1 << 7;

        explicit Parser(Lexer* lexer, Arena* arena) : 
          m_lexer(lexer),
          m_arena(arena) {}

        ast::Node* parseProgram();
        ast::Node* parseBlock();
        bool peekVisibility();
        ast::Node* parseDeclaration();
        ast::Node* parseFunction(unsigned int flags);
        ast::Node* parseClass(unsigned int flags);
        bool parseParameterList(ast::NodeList* list);
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
        Arena* const m_arena;

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
    }; //class Parser
  } //namespace internal
} //namespace brutus
#endif
