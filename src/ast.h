#ifndef BRUTUS_AST_H_
#define BRUTUS_AST_H_

#include "brutus.h"
#include "lexer.h"

#include <cstring> //for std::memcpy

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

      #define NODE_ID(x) (reinterpret_cast<intptr_t>(x))
      #define NODE_NAME(x) "n" << NODE_ID(x)

      class Node {
        public:
          explicit Node() {}
          virtual ~Node() {}
          virtual void print(std::ostream& out) const = 0;
          virtual void printDOT(std::ostream& out) const {
            out << NODE_NAME(this) << " [shape=box];" << std::endl;
          }
          virtual Kind kind() const = 0;
        private:
          DISALLOW_COPY_AND_ASSIGN(Node);
      };

      class NodeWithValue : public Node {
        public:
          explicit NodeWithValue() : m_value(nullptr), m_length(0) {}

          void copyValue(Lexer* lexer) {
            // Allocate a buffer with the given value's length.
            // We add +1 to that length to store a terminating 0
            // character.

            m_length = lexer->valueLength();
            m_value = NewArray<char>(m_length + 1); //TODO(joa): arena
            m_value[m_length] = '\0';

            std::memcpy(m_value, lexer->value(), sizeof(char) * m_length);
          }

          char* value() const { return m_value; }
          size_t length() const { return m_length; }
        private:
          DISALLOW_COPY_AND_ASSIGN(NodeWithValue);
        protected:
          char* m_value;
          size_t m_length;
      };

      class NodeList {
        public:
          explicit NodeList() {
            m_nodesIndex = 0;
            m_nodesSize = 8;
            m_nodes = NewArray<Node*>(m_nodesSize); //TODO(joa): arena
          }

          void add(ast::Node* node) {
            if(m_nodesIndex == m_nodesSize) {
              auto newSize = m_nodesSize << 1;
              auto newNodes = NewArray<Node*>(newSize); //TODO(joa): arena

              std::memcpy(newNodes, m_nodes, sizeof(ast::Node*) * m_nodesSize);

              DeleteArray(m_nodes); //TODO(joa): arena

              m_nodes = newNodes;
              m_nodesSize = newSize;
            }

            m_nodes[m_nodesIndex] = node;
            ++m_nodesIndex;
          }

          void print(std::ostream& out, bool newLine) const { 
            out << '['; 
            if(m_nodesIndex > 0) {
              auto ptr = m_nodes;
              auto n = *ptr++;

              if(nullptr == n) {
                out << "null";
              } else {
                n->print(out);
              }

              for(size_t i = 1; i < m_nodesIndex; ++i) {
                out << ',';

                if(newLine) {
                  out << std::endl;
                }

                n = *ptr++;

                if(nullptr == n) {
                  out << "null";
                } else {
                  n->print(out);
                }
              }
            }
            out << ']';
          }

          void printDOT(std::ostream& out) const {
            if(m_nodesIndex > 0) {
              auto ptr = m_nodes;
              auto n = *ptr++;

              if(nullptr != n) {
                n->printDOT(out);
              }

              for(size_t i = 1; i < m_nodesIndex; ++i) {
                n = *ptr++;

                if(nullptr != n) {
                  n->printDOT(out);
                }
              }
            }
          }

          template<class T>
          void printDOTConnections(std::ostream& out, T* fromNode) const {
            if(m_nodesIndex > 0) {
              auto ptr = m_nodes;
              auto n = *ptr++;

              if(nullptr != n) {
                out << NODE_NAME(fromNode) << " -> " NODE_NAME(n) << ';' << std::endl;
              }

              for(size_t i = 1; i < m_nodesIndex; ++i) {
                n = *ptr++;

                if(nullptr != n) {
                  out << NODE_NAME(fromNode) << " -> " NODE_NAME(n) << ';' << std::endl;
                }
              }
            }
          }

          size_t size() const { 
            return m_nodesIndex;
          }

          Node** nodes() const {
            return m_nodes;
          }

          bool nonEmpty() const {
            return m_nodesIndex > 0;
          }
        private:
          DISALLOW_COPY_AND_ASSIGN(NodeList);
          Node** m_nodes;
          size_t m_nodesSize, m_nodesIndex;
      };

      class Error : public Node {
        public:
          explicit Error() : m_value(nullptr), m_line(0), m_column(0) {}

          void init(const char* value, unsigned int line, unsigned int column) {
            m_value = value;
            m_line = line;
            m_column = column;
          }

          void print(std::ostream& out) const {
            out << "Error(" << m_value << " (line " << m_line << ", col " << m_column << "))";
          }

          void printDOT(std::ostream& out) const {
            out << NODE_NAME(this) << " [shape=box, label=\"Error\"];" << std::endl;
          }

          Kind kind() const { return ERROR; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Error);
          const char* m_value;
          unsigned int m_line;
          unsigned int m_column;
      };

      class Identifier : public NodeWithValue {
        public:
          explicit Identifier() {}
          void print(std::ostream& out) const { out << "Identifier(" << m_value << ')'; }
          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << "[shape=box, label=\"Identifier(" << m_value << ")\"];" << std::endl; }
          Kind kind() const { return IDENTIFIER; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Identifier);
      };

      class Number : public NodeWithValue {
        public:
          explicit Number() {}
          void print(std::ostream& out) const { out << "Number(" << m_value << ')'; }
          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << "[shape=box, label=\"Number(" << m_value << ")\"];" << std::endl; }
          Kind kind() const { return NUMBER; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Number);
      };

      class String : public NodeWithValue {
        public:
          explicit String() {}
          void print(std::ostream& out) const { out << "String(" << m_value << ')'; }
          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << "[shape=box, label=\"String(" << m_value << ")\"];" << std::endl; }
          Kind kind() const { return STRING; }
        private:
          DISALLOW_COPY_AND_ASSIGN(String);
      };

      class This : public Node {
        public:
          explicit This() {}
          void print(std::ostream& out) const { out << "This"; }
          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << "[shape=box, label=This];" << std::endl; }
          Kind kind() const { return THIS; }
        private:
          DISALLOW_COPY_AND_ASSIGN(This);
      };

      class Block : public Node {
        public:
          explicit Block() {}

          size_t size() const { 
            return m_list.size();
          }

          void add(ast::Node* node) {
            m_list.add(node);
          }

          void print(std::ostream& out) const {
            out << "Block(";
            m_list.print(out, true);
            out << ')';
          }

          void printDOT(std::ostream& out) const {
            out << NODE_NAME(this) << " [shape=box, label=Block];" << std::endl;
            m_list.printDOT(out);
            m_list.printDOTConnections(out, this);
          }

          Kind kind() const { return BLOCK; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Block);
          NodeList m_list;
      };

      class Variable : public Node {
        public:
          explicit Variable() : m_isModifiable(NO), m_name(nullptr), m_type(nullptr), m_init(nullptr) {}
          
          void init(bool isModifiable, Node* name, Node* type, Node* init) {
            m_isModifiable = isModifiable;
            m_name = name;
            m_type = type;
            m_init = init;
          }

          void print(std::ostream& out) const {
            out << "Variable(" << (m_isModifiable ? "VAR" : "VAL") << ',';
            m_name->print(out);
            out << ',';
            
            if(nullptr != m_type) {
              m_type->print(out);
              out << ',';
            } else {
              out << "NO TYPE,";
            }

            if(nullptr != m_init) {
              m_init->print(out);
              out << ')';
            } else {
              out << "NO INIT)";
            }
          }

          void printDOT(std::ostream& out) const {
            out << NODE_NAME(this) << " [shape=box, label=Variable];" << std::endl;

            if(nullptr != m_type) {
              m_type->printDOT(out);
              out << NODE_NAME(this) << " -> " << NODE_NAME(m_type) << "[label=Type];" << std::endl;
            }

            if(nullptr != m_init) {
              m_init->printDOT(out);
              out << NODE_NAME(this) << " -> " << NODE_NAME(m_init) << "[label=Init];" << std::endl;
            }
          }

          Kind kind() const { return VARIABLE; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Variable);
          bool m_isModifiable;
          Node* m_name;
          Node* m_type;
          Node* m_init;
      };

      class True : public Node {
        public:
          explicit True() {}
          void print(std::ostream& out) const { out << "True"; }
          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << " [shape=box, label=True];" << std::endl; }
          Kind kind() const { return TRUE_; }
        private:
          DISALLOW_COPY_AND_ASSIGN(True);
        };

        class False : public Node {
        public:
          explicit False() {}
          void print(std::ostream& out) const { out << "False"; }
          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << " [shape=box, label=False];" << std::endl; }
          Kind kind() const { return FALSE_; }
        private:
          DISALLOW_COPY_AND_ASSIGN(False);
      };

      class Select : public Node {
        public: 
          explicit Select() : m_object(nullptr), m_qualifier(nullptr) {}
          void init(Node* object, Node* qualifier) {
            m_object = object;
            m_qualifier = qualifier;
          }
          void print(std::ostream& out) const {
            out << "Select(";
            m_object->print(out);
            out << ',';
            m_qualifier->print(out);
            out << ')';
          }
          void printDOT(std::ostream& out) const {
            out << NODE_NAME(this) << " [shape=box, label=Select];" << std::endl;
            m_object->printDOT(out);
            m_qualifier->printDOT(out);
            out << NODE_NAME(this) << " -> " << NODE_NAME(m_object) << " [label=Object];" << std::endl;
            out << NODE_NAME(this) << " -> " << NODE_NAME(m_qualifier) << " [label=Qualifier];" << std::endl;
          }
          Kind kind() const { return SELECT; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Select);
          Node* m_object;
          Node* m_qualifier;
      };

      class If : public Node {
        public:
          explicit If() {}

          NodeList* cases() {
            return &m_cases;
          }

          void print(std::ostream& out) const {
            out << "If(";
            m_cases.print(out, true);
            out << ")";
          }
          void printDOT(std::ostream& out) const {
            out << NODE_NAME(this) << " [shape=box, label=If];" << std::endl;
            m_cases.printDOT(out);
            m_cases.printDOTConnections(out, this);
          }
          Kind kind() const { return IF; }
        private:
          DISALLOW_COPY_AND_ASSIGN(If);
          NodeList m_cases;
      };

      class IfCase : public Node {
        public:
          explicit IfCase() : m_condition(nullptr), m_block(nullptr) {}
          
          void init(Node* condition, Node* block) {
            m_condition = condition;
            m_block = block;
          }

          void print(std::ostream& out) const {
            out << "IfCase(";
            m_condition->print(out);
            out << ',';
            m_block->print(out);
            out << ")";
          }
          
          void printDOT(std::ostream& out) const {
            out << NODE_NAME(this) << " [shape=box, label=IfCase];" << std::endl;
            m_condition->printDOT(out);
            m_block->printDOT(out);
            out << NODE_NAME(this) << " -> " << NODE_NAME(m_condition) << " [label=Condition];" << std::endl;
            out << NODE_NAME(this) << " -> " << NODE_NAME(m_block) << " [label=Block];" << std::endl;
          }

          Kind kind() const { return IF_CASE; }
        private:
          DISALLOW_COPY_AND_ASSIGN(IfCase);
          Node* m_condition;
          Node* m_block;
      };

      class Call : public Node {
        public: 
          explicit Call() : m_callee(nullptr) {}
          void init(Node* callee) {
            m_callee = callee;
          }

          NodeList* arguments() {
            return &m_arguments;
          }

          void print(std::ostream& out) const {
            out << "Call(";
            m_callee->print(out);
            out << ',';
            m_arguments.print(out, false);
            out << ')';
          }

          void printDOT(std::ostream& out) const {
            m_callee->printDOT(out);
            out << NODE_NAME(this) << " [shape=box, label=Call];" << std::endl;
            out << NODE_NAME(this) << " -> " NODE_NAME(m_callee) << " [label=Callee];" << std::endl;
            m_arguments.printDOT(out);
            m_arguments.printDOTConnections(out, this);
          }
          
          Kind kind() const { return CALL; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Call);
          Node* m_callee;
          NodeList m_arguments;
      };

      class Argument : public Node {
        public:
          explicit Argument() : m_name(nullptr), m_value(nullptr) {}

          void init(Node* name, Node* value) {
            m_name = name;
            m_value = value;
          }

          void print(std::ostream& out) const {
            out << "Argument(";
            if(nullptr != m_name) {
              m_name->print(out);
              out << '=';
            }
            m_value->print(out);
            out << ')';
          }

          void printDOT(std::ostream& out) const { 
            out << NODE_NAME(this) << " [shape=box, label=Argument];" << std::endl;
            if(nullptr != m_name) {
              m_name->printDOT(out);
              out << NODE_NAME(this) << " -> " << NODE_NAME(m_name) << " [label=Name];" << std::endl;
            }
            m_value->printDOT(out);
            out << NODE_NAME(this) << " -> " << NODE_NAME(m_value) << " [label=Value]" << std::endl;
          }

          Kind kind() const { return ARGUMENT; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Argument);
          Node* m_name;
          Node* m_value;
      };

      class Function : public Node {
        public:
          explicit Function() : m_name(nullptr), m_type(nullptr), m_block(nullptr), m_flags(0) {}

          void init(Node* name, Node* type, Node* block, unsigned int flags) {
            m_name = name;
            m_type = type;
            m_block = block;
            m_flags = flags;
          }

          void print(std::ostream& out) const {
            out << "Function(";
            if(nullptr == m_name) {
              out << "<anonymous>";
            } else {
              m_name->print(out);
            }
            out << ',';
            m_typeParameters.print(out, false);
            out << ',';
            m_parameters.print(out, false);
            out << ',';
            if(nullptr != m_type) {
              m_type->print(out);
            } else {
              out << "null";
            }
            out << ',';
            m_block->print(out);
            out << ')';
          }
          
          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << " [shape=box, label=Function];" << std::endl; }
          
          NodeList* parameters() {
            return &m_parameters;
          }

          NodeList* typeParameters() {
            return &m_typeParameters;
          }

          Kind kind() const { return FUNCTION; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Function);
          Node* m_name;
          Node* m_type;
          Node* m_block;
          unsigned int m_flags;
          NodeList m_typeParameters;
          NodeList m_parameters;
      };

      class Parameter : public Node {
        public:
          explicit Parameter() : m_name(nullptr), m_type(nullptr) {}
          
          void init(Node* name, Node* type) {
            m_name = name;
            m_type = type;
          }

          void print(std::ostream& out) const {
            out << "Parameter(";
            m_name->print(out);
            if(nullptr != m_type) {
              out << ',';
              m_type->print(out);
            }
            out << ')';
          }

          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << " [shape=box, label=Parameter];" << std::endl; }

          Kind kind() const { return PARAMETER; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Parameter);
          Node* m_name;
          Node* m_type;
      };

      class TypeParameter : public Node  {
        public:
          explicit TypeParameter() : m_name(nullptr), m_bound(nullptr), m_boundType(0) {}

          void init(Node* name, Node* bound, unsigned int boundType) {
            m_name = name;
            m_bound = bound;
            m_boundType = boundType;
          }

          void print(std::ostream& out) const {
            out << "TypeParameter(";
            m_name->print(out);
            if(m_boundType != 0) {
              out << ',';
              m_bound->print(out);
              out << ',' << m_boundType;
            }
            out << ')';
          }
          
          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << " [shape=box, label=TypeParameter];" << std::endl; }

          Kind kind() const { return TYPE_PARAMETER; }
        private:
          DISALLOW_COPY_AND_ASSIGN(TypeParameter);
          Node* m_name;
          Node* m_bound;
          unsigned int m_boundType;
      };

      class Class : public Node {
        public:
          explicit Class() : m_name(nullptr), m_flags(0) {}

          void init(Node* name, unsigned int flags) {
            m_name = name;
            m_flags = flags;
          }

          void print(std::ostream& out) const {
            out << "Class(";
            m_name->print(out);
            out << ',';
            m_typeParameters.print(out, false);
            out << ',';
            m_members.print(out, true);
            out << ')';
          }
          
          void printDOT(std::ostream& out) const { out << NODE_NAME(this) << " [shape=box, label=Class];" << std::endl; }

          NodeList* typeParameters() {
            return &m_typeParameters;
          }

          NodeList* members() {
            return &m_members;
          }

          Kind kind() const { return CLASS; }
        private:
          DISALLOW_COPY_AND_ASSIGN(Class);
          Node* m_name;
          unsigned int m_flags;
          NodeList m_typeParameters;
          NodeList m_members;
      };
      #undef NODE_ID
    } //namespace ast
  } //namespace internal
} //namespace brutus
#endif
