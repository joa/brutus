#include "ast.h"

namespace brutus {
namespace internal {
namespace ast {
Error::Error() : 
  m_value(nullptr), 
  m_line(0), 
  m_column(0) {}

void Error::init(const char* value, unsigned int line, unsigned int column) {
  m_value = value;
  m_line = line;
  m_column = column;
}

void Error::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Error::kind() const {
  return ERROR;
}

const char* Error::value() const {
  return m_value;
}

unsigned int Error::line() const {
  return m_line;
}

unsigned int Error::column() const {
  return m_column;
}

//

Identifier::Identifier() : 
  m_name(nullptr) {}

void Identifier::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Identifier::kind() const {
  return IDENTIFIER;
}

void Identifier::init(Name* name) {
  m_name = name;
}

Name* Identifier::name() const {
  return m_name;
}

//

Number::Number() : 
  m_name(nullptr) {}

void Number::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Number::kind() const {
  return NUMBER;
}

void Number::init(Name* name) {
  m_name = name;
}

Name* Number::name() const {
  return m_name;
}

//

String::String() : 
  m_name(nullptr) {}

void String::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind String::kind() const {
  return STRING;
}

void String::init(Name* name) {
  m_name = name;
}

Name* String::name() const {
  return m_name;
}

//

This::This() {}

void This::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind This::kind() const {
  return THIS;
}

//

Block::Block() {}

void Block::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Block::kind() const {
  return BLOCK;
}

NodeList* Block::expressions() {
  return &m_expressions;
}

//

True::True() {}

void True::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind True::kind() const {
  return TRUE_;
}

//

False::False() {}

void False::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind False::kind() const {
  return FALSE_;
}

//

Select::Select() : 
  m_object(nullptr), 
  m_qualifier(nullptr) {}

void Select::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Select::kind() const {
  return SELECT;
}

void Select::init(Node* object, Node* qualifier) {
  m_object = object;
  m_qualifier = qualifier;
}

Node* Select::object() const {
  return m_object;
}

Node* Select::qualifier() const {
  return m_qualifier;
}

//

If::If() {}

void If::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind If::kind() const {
  return IF;
}

NodeList* If::cases() {
  return &m_cases;
}


//

IfCase::IfCase()
    : m_condition(nullptr),
      m_expr(nullptr) {}

void IfCase::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind IfCase::kind() const {
  return IF_CASE;
}

void IfCase::init(Node* condition, Node* expr) {
  m_condition = condition;
  m_expr = expr;
}

Node* IfCase::condition() const {
  return m_condition;
}

Node* IfCase::expr() const {
  return m_expr;
}

//

Call::Call()
    : m_callee(nullptr) {}

void Call::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Call::kind() const {
  return CALL;
}

void Call::init(Node* callee) {
  m_callee = callee;
}

Node* Call::callee() const {
  return m_callee;
}

NodeList* Call::arguments() {
  return &m_arguments;
}

//

Argument::Argument()
    : m_name(nullptr),
      m_value(nullptr) {}

void Argument::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Argument::kind() const {
  return ARGUMENT;
}

void Argument::init(Node* name, Node* value)  {
  m_name = name;
  m_value = value;
}

bool Argument::hasName() const {
  return m_name != nullptr;
}

Node* Argument::name() const {
  return m_name;
}

Node* Argument::value() const {
  return m_value;
}

//

Variable::Variable()
    : m_isModifiable(NO),
      m_name(nullptr),
      m_type(nullptr),
      m_init(nullptr) {}

void Variable::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Variable::kind() const {
  return VARIABLE;
}

void Variable::init(bool isModifiable, Node* name, Node* type, Node* init) {
  m_isModifiable = isModifiable;
  m_name = name;
  m_type = type;
  m_init = init;
}

bool Variable::isModifiable() const {
  return m_isModifiable;
}

bool Variable::hasInit() const {
  return m_init != nullptr;
}

bool Variable::hasType() const {
  return m_type != nullptr;
}

Node* Variable::name() const {
  return m_name;
}

Node* Variable::type() const {
  return m_type;
}

Node* Variable::init() const {
  return m_init;
}

//

Function::Function()
    : m_name(nullptr), 
      m_type(nullptr), 
      m_expr(nullptr), 
      m_flags(0) {}

void Function::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Function::kind() const {
  return FUNCTION;
}

void Function::init(Node* name, Node* type, Node* expr, unsigned int flags) {
  m_name = name;
  m_type = type;
  m_expr = expr;
  m_flags = flags;
}

NodeList* Function::parameters() {
  return &m_parameters;
}

NodeList* Function::typeParameters() {
  return &m_typeParameters;
}

bool Function::isAnonymous() const {
  return m_name == nullptr;
}

bool Function::hasType() const {
  return m_type != nullptr;
}

Node* Function::name() const {
  return m_name;
}

Node* Function::type() const {
  return m_type;
}

Node* Function::expr() const {
  return m_expr;
}

bool Function::isAbstract() const {
  return m_expr == nullptr;
}

//

Parameter::Parameter()
    : m_name(nullptr),
      m_type(nullptr) {}

void Parameter::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Parameter::kind() const {
  return PARAMETER;
}

void Parameter::init(Node* name, Node* type) {
  m_name = name;
  m_type = type;
}

Node* Parameter::name() const {
  return m_name;
}

Node* Parameter::type() const {
  return m_type;
}

bool Parameter::hasType() const {
  return m_type != nullptr;
}

//

TypeParameter::TypeParameter()
    : m_name(nullptr), 
      m_bound(nullptr), 
      m_boundType(0) {}

void TypeParameter::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind TypeParameter::kind() const {
  return TYPE_PARAMETER;
}

void TypeParameter::init(Node* name, Node* bound, unsigned int boundType) {
  m_name = name;
  m_bound = bound;
  m_boundType = boundType;
}

Node* TypeParameter::name() const {
  return m_name;
}

Node* TypeParameter::bound() const {
  return m_bound;
}

unsigned int TypeParameter::boundType() const {
  return m_boundType;
}

//

Class::Class() : 
  m_name(nullptr), 
  m_flags(0) {}

void Class::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Class::kind() const {
  return CLASS;
}

void Class::init(Node* name, unsigned int flags) {
  m_name = name;
  m_flags = flags;
}

Node* Class::name() const {
  return m_name;
}

unsigned int Class::flags() const {
  return m_flags;
}

NodeList* Class::typeParameters() {
  return &m_typeParameters;
}

NodeList* Class::members() {
  return &m_members;
}

//

Module::Module()
    : m_name(nullptr) {}

void Module::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Module::kind() const {
  return MODULE;
}

void Module::init(Node* name) {
  m_name = name;
}

Node* Module::name() const {
  return m_name;
}

NodeList* Module::declarations() {
  return &m_declarations;
}

NodeList* Module::dependencies() {
  return &m_dependencies;
}

//

ModuleDependency::ModuleDependency() 
    : m_name(nullptr),
      m_version(nullptr) {}

void ModuleDependency::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind ModuleDependency::kind() const {
  return MODULE_DEPENDENCY;
}

void ModuleDependency::init(Node* name, Node* version) {
  m_name = name;
  m_version = m_version;
}

Node* ModuleDependency::name() const {
  return m_name;
}

Node* ModuleDependency::version() const {
  return m_version;
}

bool ModuleDependency::hasVersion() const {
  return m_version != nullptr;
}

//

Program::Program() {}

void Program::accept(ASTVisitor* visitor) {
  visitor->visit(this);
}

Kind Program::kind() const {
  return PROGRAM;
}

NodeList* Program::modules() {
  return &m_modules;
}

//

ASTVisitor::ASTVisitor() {}

void ASTVisitor::visit(Argument* node) {
  if(node->hasName()) {
    node->name()->accept(this);
  }

  node->value()->accept(this);
}

void ASTVisitor::visit(Block* node) {
  acceptAll(node->expressions());
}

void ASTVisitor::visit(Call* node) {
  node->callee()->accept(this);
  acceptAll(node->arguments());
}

void ASTVisitor::visit(Class* node) {
  node->name()->accept(this);
  acceptAll(node->typeParameters());
  acceptAll(node->members());
}

void ASTVisitor::visit(Error* node) {
  UNUSED(node);
}

void ASTVisitor::visit(False* node) {
  UNUSED(node);
}

void ASTVisitor::visit(Function* node) {
  if(!node->isAnonymous()) {
    node->name()->accept(this);
  }

  if(node->hasType()) {
    node->type()->accept(this);
  }

  acceptAll(node->typeParameters());
  acceptAll(node->parameters());
  node->expr()->accept(this);
}

void ASTVisitor::visit(Identifier* node) {
  UNUSED(node);
}

void ASTVisitor::visit(If* node) {
  acceptAll(node->cases());
}

void ASTVisitor::visit(IfCase* node) {
  node->condition()->accept(this);
  node->expr()->accept(this);
}

void ASTVisitor::visit(Module* node) {
  node->name()->accept(this);
  acceptAll(node->declarations());
  acceptAll(node->dependencies());
}

void ASTVisitor::visit(ModuleDependency* node) {
  node->name()->accept(this);

  if(node->hasVersion()) {
    node->version()->accept(this);
  }
}

void ASTVisitor::visit(Number* node) {
  UNUSED(node);
}

void ASTVisitor::visit(Parameter* node) {
  node->name()->accept(this);

  if(node->hasType()) {
    node->type()->accept(this);
  }
}


void ASTVisitor::visit(Program* node) {
  acceptAll(node->modules());
}

void ASTVisitor::visit(Select* node) {
  node->object()->accept(this);
  node->qualifier()->accept(this);
}

void ASTVisitor::visit(String* node) {
  UNUSED(node);
}

void ASTVisitor::visit(This* node) {
  UNUSED(node);
}

void ASTVisitor::visit(True* node) {
  UNUSED(node);
}

void ASTVisitor::visit(TypeParameter* node) {
  node->name()->accept(this);
  node->bound()->accept(this);
}

void ASTVisitor::visit(Variable* node) {
  node->name()->accept(this);

  if(node->hasType()) {
    node->type()->accept(this);
  }

  if(node->hasInit()) {
    node->init()->accept(this);
  }
}

void ASTVisitor::acceptAll(NodeList* list) {
  list->foreach([&](Node* node) -> void {
    node->accept(this);
  });
}

//

static const char* Indentation[] = {
  "",
  "  ",
  "    ",
  "      ",
  "        ",
  "          ",
  "            "
};

static const int MaxIndentation = NumberOfElements(Indentation);

ASTPrinter::ASTPrinter(std::ostream& output)  //NOLINT
    : m_output(output), 
      m_indentLevel(0),
      m_wasNewLine(NO) {}

void ASTPrinter::print(Node* node) {
  node->accept(this);
}

void ASTPrinter::pushIndent() {
  ++m_indentLevel;

  if(m_indentLevel >= MaxIndentation) {
    m_indentLevel = MaxIndentation - 1;
  }
}

void ASTPrinter::popIndent() {
  --m_indentLevel;

  if(m_indentLevel < 0) {
    m_indentLevel = 0;
  }
}

template<typename T>
void ASTPrinter::print(T value) {
  maybeIndent();
  m_output << value;
}

template<typename T>
void ASTPrinter::println(T value) {
  print(value);
  nl();
}

void ASTPrinter::nl() {
  m_output << std::endl;
  m_wasNewLine = YES;
}

void ASTPrinter::maybeIndent() {
  if(m_wasNewLine) {
    m_output << Indentation[m_indentLevel];
    m_wasNewLine = NO;
  }
}

void ASTPrinter::printAll(NodeList* nodes, const char* separatorChars) {
  const int m = nodes->size() - 1;
  int i = 0;
  
  nodes->foreach([&](Node* node) {
    node->accept(this);
    if(i++ != m) {
      print(separatorChars);
    }
  });
}

void ASTPrinter::visit(Argument* node) {
  if(node->hasName()) {
    node->name()->accept(this);
    print(" = ");
  }

  node->value()->accept(this);
}

void ASTPrinter::visit(Block* node) {
  print('{');
  nl();
  pushIndent();
  node->expressions()->foreach([&](Node* node) {
    node->accept(this);
    nl();
  });
  popIndent();
  print('}');
}

void ASTPrinter::visit(Call* node) {
  node->callee()->accept(this);
  print("(");
  printAll(node->arguments(), ", ");
  print(')');
}

void ASTPrinter::visit(Class* node) {
  const int m = node->members()->size() - 1;
  int i = 0;
  
  print("class ");
  node->name()->accept(this);
  print(" {");
  nl();
  pushIndent();
  node->members()->foreach([&](Node* node) {
    node->accept(this);
    nl();

    if(i++ != m) {
      nl();
    }
  });
  popIndent();
  print('}');
}

void ASTPrinter::visit(Error* node) {
  print("<<error(");
  print(node->value());
  print(", ");
  print(node->line());
  print(':');
  print(node->column());
  print(")>>");
}

void ASTPrinter::visit(False* node) {
  UNUSED(node);
  print("false");
}

void ASTPrinter::visit(Function* node) {
  if(node->isAnonymous()) {
    //TODO(joa): type etc.
    print("{ ");
    printAll(node->parameters(), ", ");
    print(" -> ");
    node->expr()->accept(this);
    print(" }");
  } else {
    print("def ");
    node->name()->accept(this);
    
    if(node->typeParameters()->nonEmpty()) {
      print('[');
      printAll(node->typeParameters(), ", ");
      print(']');
    }
    
    print('(');
    printAll(node->parameters(), ", ");
    
    if(node->hasType()) {
      print("): ");
      node->type()->accept(this);
      
      if(!node->isAbstract()) {
        print(" =");
      }
    } else {
      print(')');
    }
    
    if(!node->isAbstract()) {
      print(' ');
      node->expr()->accept(this);
    }
  }
}

void ASTPrinter::visit(Identifier* node) {
  print(node->name()->value());
}

void ASTPrinter::visit(If* node) {
  print("if ");
  if(node->cases()->size() == 1) {
    node->cases()->get(0)->accept(this);
  } else {
    print('{');
    nl();
    pushIndent();
    node->cases()->foreach([&](Node* node) {
      node->accept(this);
      nl();
    });
    popIndent();
    print("}");
  }
}

void ASTPrinter::visit(IfCase* node) {
  node->condition()->accept(this);
  print(" -> ");
  node->expr()->accept(this);
}

void ASTPrinter::visit(Module* node) {
  print("module ");
  node->name()->accept(this);

  //TODO(joa): fix this!
  if(node->name()->kind() == ast::IDENTIFIER) {
    ast::Identifier* name = static_cast<ast::Identifier*>(node->name());

    if(name->name()->length() > 0) {
      print(" {");
    } else {
      print('{');
    }
  } else {
    print(" {");
  }
  nl();
  pushIndent();
  
  node->dependencies()->foreach([&](Node* node) {
    node->accept(this);
    nl();
  });
  
  if(node->dependencies()->nonEmpty()) {
    nl();
  }
  
  const int m = node->declarations()->size() - 1;
  int i = 0;

  node->declarations()->foreach([&](Node* node) {
    node->accept(this);
    nl();

    if(i++ != m) {
      nl();
    }
  }); 

  popIndent();
  print('}');
  nl();
}

void ASTPrinter::visit(ModuleDependency* node) {
  print("require ");
  node->name()->accept(this);

  if(node->hasVersion()) {
    print(": ");
    node->version()->accept(this);
  }
}

void ASTPrinter::visit(Number* node) {
  print(node->name()->value());
}

void ASTPrinter::visit(Parameter* node) {
  node->name()->accept(this);

  if(node->hasType()) {
    print(": ");
    node->type()->accept(this);
  }
}

void ASTPrinter::visit(Program* node) {
  node->modules()->foreach([&](Node* node) {
    node->accept(this);
    nl();
  });
}

void ASTPrinter::visit(Select* node) {
  node->object()->accept(this);
  print('.');
  node->qualifier()->accept(this);
}

void ASTPrinter::visit(String* node) {
  print('"');
  //TODO(joa): escape
  print(node->name()->value());
  print('"');
}

void ASTPrinter::visit(This* node) {
  UNUSED(node);
  print("this");
}

void ASTPrinter::visit(True* node) {
  UNUSED(node);
  print("true");
}

void ASTPrinter::visit(TypeParameter* node) {
  auto boundType = node->boundType();

  print(node->name());

  if(1 == boundType) {
    print(" -> ");
    print(node->bound());
  } else if(2 == boundType) {
    print(" <- ");
    print(node->bound());
  }
}

void ASTPrinter::visit(Variable* node) {
  print(node->isModifiable() ? "var " : "val ");
  node->name()->accept(this);

  if(node->hasType()) {
    print(": ");
    node->type()->accept(this);
  }

  if(node->hasInit()) {
    print(" = ");
    node->init()->accept(this);
  }
}

// 

NodeList::NodeList() : 
  m_nodes(nullptr),
  m_nodesSize(0),
  m_nodesIndex(0) {}

void NodeList::add(Node* node, Arena* arena) {
  if(m_nodesIndex == m_nodesSize) {
    static const size_t kNodePointerSize = sizeof(Node*); //NOLINT
    auto newSize = m_nodesSize == 0 ? 1 : m_nodesSize << 1;
    auto newNodes = arena->newArray<Node*>(newSize);

    if(m_nodes != nullptr) {
      // The nodes array is only null if the NodeList is fresh
      // and no node has been added yet.
      ArrayCopy(newNodes, m_nodes, kNodePointerSize * m_nodesSize);
    }

    m_nodes = newNodes;
    m_nodesSize = newSize;
  }

  m_nodes[m_nodesIndex] = node;
  ++m_nodesIndex;
}

int NodeList::size() const { 
  return m_nodesIndex;
}

Node* NodeList::get(const int& index) {
  if(index >= m_nodesIndex || index < 0) {
#ifdef DEBUG
    std::cerr << "Warning: Array element " << 
        index << " is out of bounds." << std::endl;
#endif
    return nullptr;
  }

  return m_nodes[index];
}

Node** NodeList::nodes() const {
  return m_nodes;
}

bool NodeList::nonEmpty() const {
  return m_nodesIndex > 0;
}

void NodeList::foreach(std::function<void(Node*)> f) { //NOLINT
  const auto n = m_nodesIndex;
  auto nodes = m_nodes;

  for(int i = 0; i < n; ++i) {
    auto node = *nodes++;

    if(nullptr != node) {
      f(node);
    }
  }
}

bool NodeList::forall(std::function<bool(Node*)> f) { //NOLINT
  const auto n = m_nodesIndex;
  auto nodes = m_nodes;

  for(int i = 0; i < n; ++i) {
    auto node = *nodes++;

    if(nullptr != node && !f(node)) {
      return false;
    }
  }

  return true;
}
} //namespace ast
} //namespace internal
} //namespace brutus
