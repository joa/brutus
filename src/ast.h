#ifndef _AST_H_
#define _AST_H_

enum Kind {
  ERROR,
  BLOCK,
  IDENTIFIER,
  THIS,
  IF,
  VARIABLE,
  TRUE_,
  FALSE_
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
    m_value = new char[m_length + 1];
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

class Error : public Node {
public:
  explicit Error() {}

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
  explicit Block() {
    m_nodesIndex = 0;
    m_nodesSize = 8;
    m_nodes = new Node*[m_nodesSize]; //TODO(joa): arena
  }

  void add(ast::Node* node) {
    if(m_nodesIndex == m_nodesSize) {
      auto newSize = m_nodesSize << 1;
      auto newNodes = new Node*[newSize]; //TODO(joa): arena;

      std::memcpy(newNodes, m_nodes, sizeof(ast::Node*) * m_nodesSize);

      delete[] m_nodes;

      m_nodes = newNodes;
      m_nodesSize = newSize;
    }

    m_nodes[m_nodesIndex] = node;
    ++m_nodesIndex;
  }

  size_t size() const { 
    return m_nodesIndex;
  }

  void print(std::ostream& out) const {
    out << "Block(";

    if(m_nodesIndex > 0) {
      auto ptr = m_nodes;
      auto n = *ptr++;

      if(nullptr == n) {
        out << "null";
      } else {
        n->print(out);
      }

      for(size_t i = 1; i < m_nodesIndex; ++i) {
        out << ',' << std::endl;

        n = *ptr++;

        if(nullptr == n) {
          out << "null";
        } else {
          n->print(out);
        }
      }
    }

    out << ')';
  }

  Kind kind() const { return BLOCK; }
private:
  DISALLOW_COPY_AND_ASSIGN(Block);
  Node** m_nodes;
  size_t m_nodesSize, m_nodesIndex;
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
  explicit Variable() {}
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

#endif