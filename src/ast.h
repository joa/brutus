#ifndef BRUTUS_AST_H_
#define BRUTUS_AST_H_

#include "brutus.h"
#include "arena.h"
#include "lexer.h"

#define NODE_OVERRIDES() \
  void accept(ASTVisitor* visitor) override final; \
  Kind kind() const override final

namespace brutus {
  namespace internal {
    namespace ast {
      enum Kind {
        ERROR,
        BLOCK,
        IDENTIFIER,
        NUMBER,
        STRING,
        THIS,
        VARIABLE,
        TRUE_,
        FALSE_,
        SELECT,
        CALL,
        ARGUMENT,
        IF,
        IF_CASE,
        FUNCTION,
        PARAMETER,
        TYPE_PARAMETER,
        CLASS
      }; //enum Kind

      class ASTVisitor;
      
      class Node : public ArenaMember {
        public:
          void* operator new(size_t size, Arena* arena) {
            return arena->alloc(size);
          }
          Node() {}
          virtual ~Node() {}
          virtual void accept(ASTVisitor* visitor) = 0;
          virtual Kind kind() const = 0;

        private:
          void* operator new(size_t size);
          bool m_force;

          DISALLOW_COPY_AND_ASSIGN(Node);
      };

      class NodeList {
        public:
          explicit NodeList();
          void add(Node* node, Arena* arena);
          int size() const;
          Node* get(const int& index);
          Node** nodes() const;
          bool nonEmpty() const;
          void foreach(std::function<void(Node*)> f); //NOLINT
          bool forall(std::function<bool(Node*)> f); //NOLINT

        private:
          Node** m_nodes;
          int m_nodesSize;
          int m_nodesIndex;

          DISALLOW_COPY_AND_ASSIGN(NodeList);
      };

      class Expr : public Node {
        public:
          explicit Expr() : m_force(false) {}
          bool force() const { return m_force; }
          void force(const bool& value) { m_force = value; }

        private:
          bool m_force;
      };

      class Declaration : public Node {
        public:
          explicit Declaration() {}

        private:
          DISALLOW_COPY_AND_ASSIGN(Declaration);
      };

      class Error : public Node {
        public:
          explicit Error();
          void init(const char* value, unsigned int line, unsigned int column);
          const char* value() const;
          unsigned int line() const;
          unsigned int column() const;
          NODE_OVERRIDES();

        private:
          const char* m_value;
          unsigned int m_line;
          unsigned int m_column;

          DISALLOW_COPY_AND_ASSIGN(Error);
      };

      class Identifier : public Expr {
        public:
          explicit Identifier();
          void init(char* value, int length);
          char* value() const;
          int length() const;
          NODE_OVERRIDES();

        private:
          char* m_value;
          int m_length;

          DISALLOW_COPY_AND_ASSIGN(Identifier);
      };

      //TODO(joa): split into type based?
      class Number : public Expr {
        public:
          explicit Number();
          void init(char* value, int length);
          char* value() const;
          int length() const;
          NODE_OVERRIDES();

        private:
          char* m_value;
          int m_length;

          DISALLOW_COPY_AND_ASSIGN(Number);
      };

      class String : public Expr {
        public:
          explicit String();
          void init(char* value, int length);
          char* value() const;
          int length() const;
          NODE_OVERRIDES();

        private:
          char* m_value;
          int m_length;

          DISALLOW_COPY_AND_ASSIGN(String);
      };

      class This : public Expr {
        public:
          explicit This();
          NODE_OVERRIDES();

        private:
          DISALLOW_COPY_AND_ASSIGN(This);
      };

      class Block : public Expr {
        public:
          explicit Block();
          NodeList* expressions();
          NODE_OVERRIDES();

        private:
          NodeList m_expressions;

          DISALLOW_COPY_AND_ASSIGN(Block);
      };

      class True : public Expr {
        public:
          explicit True();
          NODE_OVERRIDES();

        private:
          DISALLOW_COPY_AND_ASSIGN(True);
        };

      class False : public Expr {
        public:
          explicit False();
          NODE_OVERRIDES();

        private:
          DISALLOW_COPY_AND_ASSIGN(False);
      };

      class Select : public Expr {
        public: 
          explicit Select();
          void init(Node* object, Node* qualifier);
          Node* object() const;
          Node* qualifier() const;
          NODE_OVERRIDES();

        private:
          Node* m_object;
          Node* m_qualifier;
          DISALLOW_COPY_AND_ASSIGN(Select);
      };

      class If : public Expr {
        public:
          explicit If();
          NodeList* cases();
          NODE_OVERRIDES();

        private:
          NodeList m_cases;

          DISALLOW_COPY_AND_ASSIGN(If);
      };

      class IfCase : public Node {
        public:
          explicit IfCase();
          void init(Node* condition, Node* expr);
          Node* condition() const;
          Node* expr() const;
          NODE_OVERRIDES();

        private:
          Node* m_condition;
          Node* m_expr;

          DISALLOW_COPY_AND_ASSIGN(IfCase);
      };

      class Call : public Expr {
        public: 
          explicit Call();
          void init(Node* callee);
          Node* callee() const;
          NodeList* arguments();
          NODE_OVERRIDES();

        private:
          Node* m_callee;
          NodeList m_arguments;

          DISALLOW_COPY_AND_ASSIGN(Call);
      };

      class Argument : public Expr {
        public:
          explicit Argument();
          void init(Node* name, Node* value);
          bool hasName() const;
          Node* name() const;
          Node* value() const;
          NODE_OVERRIDES();

        private:
          Node* m_name;
          Node* m_value;

          DISALLOW_COPY_AND_ASSIGN(Argument);
      };

      class Variable : public Declaration {
        public:
          explicit Variable();
          void init(bool isModifiable, Node* name, Node* type, Node* init);
          bool isModifiable() const;
          bool hasInit() const;
          bool hasType() const;
          Node* name() const;
          Node* type() const;
          Node* init() const;
          NODE_OVERRIDES();

        private:
          bool m_isModifiable;
          Node* m_name;
          Node* m_type;
          Node* m_init;

          DISALLOW_COPY_AND_ASSIGN(Variable);
      };

      class Function : public Declaration {
        public:
          explicit Function();
          void init(Node* name, Node* type, Node* expr, unsigned int flags);
          Node* name() const;
          Node* type() const;
          Node* expr() const;
          unsigned int flags() const;
          NodeList* parameters();
          NodeList* typeParameters();
          bool isAnonymous() const;
          bool hasType() const;
          bool isAbstract() const;
          NODE_OVERRIDES();

        private:
          Node* m_name;
          Node* m_type;
          Node* m_expr;
          unsigned int m_flags;
          NodeList m_typeParameters;
          NodeList m_parameters;

          DISALLOW_COPY_AND_ASSIGN(Function);
      };

      class Parameter : public Declaration {
        public:
          explicit Parameter();
          void init(Node* name, Node* type);
          Node* name() const;
          Node* type() const;
          bool hasType() const;
          NODE_OVERRIDES();

        private:
          Node* m_name;
          Node* m_type;

          DISALLOW_COPY_AND_ASSIGN(Parameter);
      };

      class TypeParameter : public Declaration  {
        public:
          explicit TypeParameter();
          void init(Node* name, Node* bound, unsigned int boundType);
          Node* name() const;
          Node* bound() const;
          unsigned int boundType() const;
          NODE_OVERRIDES();

        private:
          Node* m_name;
          Node* m_bound;
          unsigned int m_boundType;

          DISALLOW_COPY_AND_ASSIGN(TypeParameter);
      };

      class Class : public Declaration {
        public:
          explicit Class();
          void init(Node* name, unsigned int flags);
          Node* name() const;
          unsigned int flags() const;
          NodeList* typeParameters();
          NodeList* members();
          NODE_OVERRIDES();

        private:
          Node* m_name;
          unsigned int m_flags;
          NodeList m_typeParameters;
          NodeList m_members;

          DISALLOW_COPY_AND_ASSIGN(Class);
      };

      class ASTVisitor {
        public:
          explicit ASTVisitor();
          virtual ~ASTVisitor() {}
          virtual void visit(Argument* node);
          virtual void visit(Block* node);
          virtual void visit(Call* node);
          virtual void visit(Class* node);
          virtual void visit(Error* node);
          virtual void visit(False* node);
          virtual void visit(Function* node);
          virtual void visit(Identifier* node);
          virtual void visit(If* node);
          virtual void visit(IfCase* node);
          virtual void visit(Number* node);
          virtual void visit(Parameter* node);
          virtual void visit(Select* node);
          virtual void visit(String* node);
          virtual void visit(This* node);
          virtual void visit(True* node);
          virtual void visit(TypeParameter* node);
          virtual void visit(Variable* node);

        protected:
          virtual void acceptAll(NodeList* list);

        private:
          DISALLOW_COPY_AND_ASSIGN(ASTVisitor);
      };

      class ASTPrinter : public ASTVisitor {
        public:
          explicit ASTPrinter(std::ostream &output); //NOLINT
          void print(Node* node);
          virtual void visit(Argument* node) override;
          virtual void visit(Block* node) override;
          virtual void visit(Call* node) override;
          virtual void visit(Class* node) override;
          virtual void visit(Error* node) override;
          virtual void visit(False* node) override;
          virtual void visit(Function* node) override;
          virtual void visit(Identifier* node) override;
          virtual void visit(If* node) override;
          virtual void visit(IfCase* node) override;
          virtual void visit(Number* node) override;
          virtual void visit(Parameter* node) override;
          virtual void visit(Select* node) override;
          virtual void visit(String* node) override;
          virtual void visit(This* node) override;
          virtual void visit(True* node) override;
          virtual void visit(TypeParameter* node) override;
          virtual void visit(Variable* node) override;

        private:
          void pushIndent();
          void popIndent();
          
          template<typename T>
          void print(T value);
          
          template<typename T>
          void println(T value);

          void nl();
          void maybeIndent();
          void printAll(NodeList* nodes, const char* separatorChars);

          std::ostream& m_output;
          int m_indentLevel;
          bool m_wasNewLine;

          DISALLOW_COPY_AND_ASSIGN(ASTPrinter);
      };
    } //namespace ast
  } //namespace internal
} //namespace brutus
#endif
