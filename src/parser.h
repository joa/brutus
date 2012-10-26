#ifndef BRUTUS_PARSER_H_
#define BRUTUS_PARSER_H_

#include "brutus.h"
#include "arena.h"
#include "ast.h"
#include "lexer.h"
#include "name.h"

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
        static const int ACC_PURE = 1 << 8;
        static const int ACC_IMMUTABLE = 1 << 9;

        explicit Parser(Lexer* lexer, NameTable* names, Arena* arena)
            :  m_lexer(lexer),
               m_names(names),
               m_arena(arena) {}

        ast::Node* parseProgram();
        ast::Node* parseModule();
        ast::Node* parseModuleDependency();
        ast::Node* parseModuleVersion();
        ast::Node* parseBlock();
        bool peekVisibility();
        ast::Node* parseDeclaration();
        ast::Node* parseFunction(unsigned int flags);
        ast::Node* parseClass(unsigned int flags);
        ast::Node* parseVariable(unsigned int flags);
        bool parseParameterList(ast::NodeList* list);
        ast::Node* parseParameter();
        ast::Node* parseExpression();
        ast::Node* parseExpression(bool allowInfixCall);
        ast::Node* parsePrimaryExpression();
        bool peekPrimaryExpression();
        ast::Node* continueWithExpression(
          ast::Node* expression, bool allowInfixCall);
        ast::Node* parseSelect(ast::Node* object);
        ast::Node* parseAssign(ast::Node* target);
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
        ast::Node* parseType();
        ast::Node* parseTypeParameterList(ast::NodeList* list);
        ast::Node* parseTypeParameter();
        ast::Node* parseIdentifier();
        
      private:
        Lexer* const m_lexer;
        NameTable* const m_names;
        Arena* const m_arena;
        Token m_currentToken;

        void advance();
        bool isIgnored(const Token& token);
        bool peek(const Token& token);
        bool poll(const Token& token);
        void pollAll(const Token& token);
        ast::Node* consume(
          const Token& token,
          std::function<ast::Node*()> f); //NOLINT
        template<class T> T* alloc();
        template<class T> T* allocWithValue();
        ast::Node* error(const char* value);

        DISALLOW_COPY_AND_ASSIGN(Parser);
    }; //class Parser
  } //namespace internal
} //namespace brutus
#endif
