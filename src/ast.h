#ifndef BRUTUS_AST_H_
#define BRUTUS_AST_H_

#include "brutus.h"
#include "arena.h"
#include "lexer.h"
#include "name.h"

#define NODE_OVERRIDES() \
  void accept(ASTVisitor* visitor) override final; \
  NodeKind kind() const override final

namespace brutus {
  namespace internal {
    namespace syms {
      class Symbol;
      class Scope;
    }

    namespace types {
      class Type;
    }

    namespace ast {
      enum class NodeKind {
        kArgument,
        kAssign,
        kBlock,
        kCall,
        kClass,
        kError,
        kFalse,
        kFunction,
        kIdentifier,
        kIf,
        kIfCase,
        kModule,
        kModuleDependency,
        kNumber,
        kParameter,
        kProgram,
        kSelect,
        kString,
        kThis,
        kTrue,
        kTypeParameter,
        kVariable
      }; //enum Kind

      class ASTVisitor;
      
      class Node : public ArenaMember {
        public:
          void* operator new(size_t size, Arena* arena) {
            return arena->alloc(size);
          }

          explicit Node() : m_symbol(nullptr) {}
          virtual ~Node() {}
          
          virtual void accept(ASTVisitor* visitor) = 0;
          virtual NodeKind kind() const = 0;
          
          syms::Symbol* symbol() const {
            return m_symbol;
          }

          void symbol(syms::Symbol* value) {
            m_symbol = value;
          }

          types::Type* type() const {
            return m_type;
          }

          void type(types::Type* value) {
            m_type = value;
          }

        private:
          syms::Symbol* m_symbol;
          types::Type* m_type;

          void* operator new(size_t size);
          
          DISALLOW_COPY_AND_ASSIGN(Node);
      };

      class NodeList {
        public:
          explicit NodeList();
          void add(Node* node, Arena* arena);
          int size() const;
          Node* get(const int& index);
          Node** nodes() const;
          Node* last() const;
          bool nonEmpty() const;
          void foreach(std::function<void(Node*)> f); //NOLINT
          bool forall(std::function<bool(Node*)> f); //NOLINT

          template<typename T>
          T* mapToArray(std::function<T(Node*)> f, Arena* arena) {
            const auto n = m_nodesIndex;
            T* result = arena->newArray<T>(n);

            auto p = m_nodes;
            auto q = result;

            for(int i = 0; i < n; ++i) {
              auto node = *p++;

              *(q++) = (nullptr != node) ? f(node) : nullptr;
            }

            return result;
          }

        private:
          Node** m_nodes;
          int m_nodesSize;
          int m_nodesIndex;

          DISALLOW_COPY_AND_ASSIGN(NodeList);
      };

      class Expr : public Node {
        public:
          explicit Expr() {}

        private:
          DISALLOW_COPY_AND_ASSIGN(Expr);
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

      class Program : public Node {
        public:
          explicit Program();
          NodeList* modules();
          NODE_OVERRIDES();

        private:
          NodeList m_modules;

          DISALLOW_COPY_AND_ASSIGN(Program);
      };

      class Module : public Declaration {
        public:
          explicit Module();
          void init(Node* name);
          Node* name() const;
          NodeList* declarations();
          NodeList* dependencies();
          NODE_OVERRIDES();

        private:
          Node* m_name;
          NodeList m_declarations;
          NodeList m_dependencies;
          DISALLOW_COPY_AND_ASSIGN(Module);
      };

      class ModuleDependency : public Declaration {
        public:
          explicit ModuleDependency();
          void init(Node* name, Node* version);
          Node* name() const;
          Node* version() const;
          bool hasVersion() const;
          NODE_OVERRIDES();

        private:
          Node* m_name;
          Node* m_version;
          DISALLOW_COPY_AND_ASSIGN(ModuleDependency);
      };

      class Identifier : public Expr {
        public:
          explicit Identifier();
          void init(Name* name);
          Name* name() const;
          syms::Symbol* symbol() const;
          void symbol(syms::Symbol* value);
          NODE_OVERRIDES();

        private:
          Name* m_name;
          syms::Symbol* m_symbol;

          DISALLOW_COPY_AND_ASSIGN(Identifier);
      };

      //TODO(joa): split into type based?
      class Number : public Expr {
        public:
          explicit Number();
          void init(Name* name);
          Name* name() const;
          NODE_OVERRIDES();

        private:
          Name* m_name;

          DISALLOW_COPY_AND_ASSIGN(Number);
      };

      class String : public Expr {
        public:
          explicit String();
          void init(Name* name);
          Name* name() const;
          NODE_OVERRIDES();

        private:
          Name* m_name;

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
          syms::Scope* scope() const;
          void scope(syms::Scope* value);
          NODE_OVERRIDES();

        private:
          NodeList m_expressions;
          syms::Scope* m_scope;

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

      class Assign : public Expr {
        public:
          explicit Assign();
          void init(Node* target, Node* value, bool force);
          Node* target() const;
          Node* value() const;
          bool force() const;
          NODE_OVERRIDES();

        private:
          Node* m_target;
          Node* m_value;
          bool m_force;

          DISALLOW_COPY_AND_ASSIGN(Assign);
      };

      class Variable : public Declaration {
        public:
          explicit Variable();
          void init(bool isModifiable, Node* name, Node* type, Node* init, bool force);
          bool isModifiable() const;
          bool hasInit() const;
          bool hasType() const;
          Node* name() const;
          Node* type() const;
          Node* init() const;
          bool force() const;
          NODE_OVERRIDES();

        private:
          bool m_isModifiable;
          bool m_force;
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
          virtual void visit(Assign* node);
          virtual void visit(Block* node);
          virtual void visit(Call* node);
          virtual void visit(Class* node);
          virtual void visit(Error* node);
          virtual void visit(False* node);
          virtual void visit(Function* node);
          virtual void visit(Identifier* node);
          virtual void visit(If* node);
          virtual void visit(IfCase* node);
          virtual void visit(Module* node);
          virtual void visit(ModuleDependency* node);
          virtual void visit(Number* node);
          virtual void visit(Parameter* node);
          virtual void visit(Program* node);
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
          virtual void visit(Assign* node) override;
          virtual void visit(Block* node) override;
          virtual void visit(Call* node) override;
          virtual void visit(Class* node) override;
          virtual void visit(Error* node) override;
          virtual void visit(False* node) override;
          virtual void visit(Function* node) override;
          virtual void visit(Identifier* node) override;
          virtual void visit(If* node) override;
          virtual void visit(IfCase* node) override;
          virtual void visit(Module* node) override;
          virtual void visit(ModuleDependency* node) override;
          virtual void visit(Number* node) override;
          virtual void visit(Parameter* node) override;
          virtual void visit(Program* node) override;
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
