#ifndef _AST_H_
#define _AST_H_

enum Kind {
  ERROR,
  BLOCK,
  IDENTIFIER,
  NUMBER,
  STRING,
  THIS,
  IF,
  VARIABLE,
  TRUE_,
  FALSE_,
  SELECT,
  CALL,
  ARGUMENT,
  BRANCH,
  BRANCH_CASE,
  ANONYMOUS_FUNCTION,
  ANONYMOUS_FUNCTION_PARAMETER
}; //enum Kind

class Node {
public:
  explicit Node() {}
  virtual ~Node() {}
  virtual void print(std::ostream& out) const = 0;    
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
  Kind kind() const { return IDENTIFIER; }
private:
  DISALLOW_COPY_AND_ASSIGN(Identifier);
};

class Number : public NodeWithValue {
public:
  explicit Number() {}
  void print(std::ostream& out) const { out << "Number(" << m_value << ')'; }
  Kind kind() const { return NUMBER; }
private:
  DISALLOW_COPY_AND_ASSIGN(Number);
};

class String : public NodeWithValue {
public:
  explicit String() {}
  void print(std::ostream& out) const { out << "String(" << m_value << ')'; }
  Kind kind() const { return STRING; }
private:
  DISALLOW_COPY_AND_ASSIGN(String);
};

class This : public Node {
public:
  explicit This() {}
  void print(std::ostream& out) const { out << "This"; }
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

  Kind kind() const { return BLOCK; }
private:
  DISALLOW_COPY_AND_ASSIGN(Block);
  NodeList m_list;
};

class If : public Node {
public:
  explicit If() : m_condition(nullptr), m_trueCase(nullptr), m_falseCase(nullptr) {}
  void init(Node* condition, Node* trueCase, Node* falseCase) {
    m_condition = condition;
    m_trueCase = trueCase;
    m_falseCase = falseCase;
  }
  void print(std::ostream& out) const {
    out << "If(";
    m_condition->print(out);
    out << ',';
    m_trueCase->print(out);

    if(nullptr != m_falseCase) {
      out << ',';
      m_falseCase->print(out);
    }

    out << ')';
  }
  Kind kind() const { return IF; }
private:
  DISALLOW_COPY_AND_ASSIGN(If);
  Node* m_condition;
  Node* m_trueCase;
  Node* m_falseCase;
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
  Kind kind() const { return TRUE_; }
private:
  DISALLOW_COPY_AND_ASSIGN(True);
};

class False : public Node {
public:
  explicit False() {}
  void print(std::ostream& out) const { out << "False"; }
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
  Kind kind() const { return SELECT; }
private:
  DISALLOW_COPY_AND_ASSIGN(Select);
  Node* m_object;
  Node* m_qualifier;
};

class Branch : public Node {
public:
  explicit Branch() {}

  NodeList* cases() {
    return &m_cases;
  }

  void print(std::ostream& out) const {
    out << "Branch(";
    m_cases.print(out, true);
    out << ")";
  }

  Kind kind() const { return BRANCH; }
private:
  DISALLOW_COPY_AND_ASSIGN(Branch);
  NodeList m_cases;
};

class BranchCase : public Node {
public:
  explicit BranchCase() : m_condition(nullptr), m_block(nullptr) {}
  
  void init(Node* condition, Node* block) {
    m_condition = condition;
    m_block = block;
  }

  void print(std::ostream& out) const {
    out << "BranchCase(";
    m_condition->print(out);
    out << ',';
    m_block->print(out);
    out << ")";
  }
  
  Kind kind() const { return BRANCH_CASE; }
private:
  DISALLOW_COPY_AND_ASSIGN(BranchCase);
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
  Kind kind() const { return ARGUMENT; }
private:
  DISALLOW_COPY_AND_ASSIGN(Argument);
  Node* m_name;
  Node* m_value;
};

class AnonymousFunction : public Node {
public:
  explicit AnonymousFunction() : m_block(nullptr) {}
  
  NodeList* parameters() {
    return &m_parameters;
  }

  void init(Node* block) {
    m_block = block;
  }

  void print(std::ostream& out) const {
    out << "AF(";
    m_parameters.print(out, false);
    out << ',';
    m_block->print(out);
    out << ')';
  }

  Kind kind() const { return ANONYMOUS_FUNCTION; }
private:
  DISALLOW_COPY_AND_ASSIGN(AnonymousFunction);
  NodeList m_parameters;
  Node* m_block;
};

class AnonymousFunctionParameter : public Node {
public:
  explicit AnonymousFunctionParameter() : m_name(nullptr), m_type(nullptr) {}
  
  void init(Node* name, Node* type) {
    m_name = name;
    m_type = type;
  }

  void print(std::ostream& out) const {
    out << "AFParameter(";
    m_name->print(out);
    if(nullptr != m_type) {
      out << ',';
      m_type->print(out);
    }
    out << ')';
  }

  Kind kind() const { return ANONYMOUS_FUNCTION_PARAMETER; }
private:
  DISALLOW_COPY_AND_ASSIGN(AnonymousFunctionParameter);
  Node* m_name;
  Node* m_type;
};
#endif
