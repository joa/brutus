#ifndef _AST_H_
#define _AST_H_

enum Type {
  ERROR,
  BLOCK,
  IDENTIFIER
}; //enum Type

class Node {
public:
  explicit Node() {}
  virtual ~Node() {}

  virtual void print(std::ostream& out) const = 0;    
  virtual Type type() const = 0;
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
    out << u8"Error(" << m_value << u8" (line " << m_line << u8", col " << m_column << u8"))";
  }

  Type type() const { return ERROR; }
private:
  DISALLOW_COPY_AND_ASSIGN(Error);
  const char* m_value;
  unsigned int m_line;
  unsigned int m_column;
};

class Identifier : public NodeWithValue {
public:
  explicit Identifier() {}

  void print(std::ostream& out) const { out << u8"Identifier(" << m_value << u8")"; }
  Type type() const { return IDENTIFIER; }
private:
  DISALLOW_COPY_AND_ASSIGN(Identifier);
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
    out << u8"Block(";

    if(m_nodesIndex > 0) {
      auto ptr = m_nodes;

      (*ptr++)->print(out);

      for(size_t i = 1; i < m_nodesIndex; ++i) {
        out << ',' << std::endl;
        (*ptr++)->print(out);
      }
    }

    out << u8")";
  }

  Type type() const { return BLOCK; }
private:
  DISALLOW_COPY_AND_ASSIGN(Block);
  Node** m_nodes;
  size_t m_nodesSize, m_nodesIndex;
};

#endif