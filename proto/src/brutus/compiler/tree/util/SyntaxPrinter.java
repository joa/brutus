package brutus.compiler.tree.util;

import brutus.compiler.name.NameTable;
import brutus.compiler.tree.*;
import brutus.compiler.tree.Class;
import brutus.compiler.tree.Error;
import brutus.compiler.util.CodeWriter;

import static brutus.compiler.util.CodeWriter.kCommaSpace;

import java.io.Writer;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

/**
 *
 */
public final class SyntaxPrinter implements TreeVisitor, CodeWriter.Mixin {
  private final CodeWriter writer;
  private final NameTable names;

  public SyntaxPrinter(final Writer writer, final NameTable names) {
    this.writer = new CodeWriter(writer);
    this.names = names;
  }

  @Override
  public CodeWriter writer() {
    return writer;
  }

  private void printAll(List<Tree> trees, final char[] separatorChars) {
    final int m = trees.size() - 1;
    final AtomicInteger i = new AtomicInteger(0); //overkill but quick hack (and i is effectively final)

    trees.forEach((final Tree tree) -> {
      tree.accept(this);
      if(i.getAndIncrement() != m) {
        print(separatorChars);
      }
    });
  }

  private void printVisibility(final Tree tree) {
    if(tree.isPublic()) {
      print("public ");
    } else if(tree.isInternal()) {
      print("internal ");
    } else if(tree.isProtected()) {
      print("protected ");
    } else if(tree.isPrivate()) {
      print("private ");
    }
  }

  private void printFlags(final Tree tree) {
    if(tree.isVirtual()) {
      print("virtual ");
    }

    if(tree.isPartial()) {
      print("partial ");
    }

    if(tree.isSealed()) {
      print("sealed ");
    }

    if(tree.isImmutable()) {
      print("immutable ");
    }

    if(tree.isPure()) {
      print("pure ");
    }

    if(tree.isIdempotent()) {
      print("idempotent ");
    }

    if(tree.isForced()) {
      print("force ");
    }

    if(tree.isNative()) {
      print("native ");
    }
  }

  @Override
  public void visit(final Argument tree) {
    tree.name.ifPresent(name -> { print(name); print(" = "); });
    printFlags(tree);
    tree.expression.accept(this);
  }

  @Override
  public void visit(final Assign tree) {
    tree.lhs.accept(this);
    print(" = ");
    printFlags(tree);
    tree.rhs.accept(this);
  }

  @Override
  public void visit(final Block tree) {
    printFlags(tree);
    println('{');
    pushIndent();
    tree.expressions.forEach(node -> { node.accept(this); printNewLine(); });
    popIndent();
    print('}');
  }

  @Override
  public void visit(final Call tree) {
    tree.callee.accept(this);
    print('(');
    printAll(tree.arguments, kCommaSpace);
    print(')');
  }

  @Override
  public void visit(final Class tree) {
    final int m = tree.members.size() - 1;
    final AtomicInteger i = new AtomicInteger(0);

    printVisibility(tree);
    printFlags(tree);
    print("class ");
    print(tree.name);
    if(!tree.typeParameters.isEmpty()) {
      print('[');
      printAll(tree.typeParameters, kCommaSpace);
      print(']');
    }
    if(!tree.bases.isEmpty()) {
      print(" : ");
      printAll(tree.bases, kCommaSpace);
    }
    println(" {");
    pushIndent();
    tree.members.forEach(member -> {
      member.accept(this);
      printNewLine();
      if(i.getAndIncrement() != m) {
        printNewLine();
      }
    });
    popIndent();
    print('}');
  }

  @Override
  public void visit(final Error tree) {
    print("<<error(");
    print(String.valueOf(tree.position.line));
    print(':');
    print(String.valueOf(tree.position.column));
    print(")>>");
  }

  @Override
  public void visit(final FunctionDeclaration tree) {
    printVisibility(tree);
    printFlags(tree);
    print("def ");
    print(tree.name);
    if(!tree.typeParameters.isEmpty()) {
      print('[');
      printAll(tree.typeParameters, kCommaSpace);
      print(']');
    }
    if(!tree.isIdempotent() || !tree.parameters.isEmpty()) {
      print('(');
      printAll(tree.parameters, kCommaSpace);
      print(')');
    }
    tree.type.ifPresent(type -> {
      print(": ");
      type.accept(this);
    });
    tree.expression.ifPresent(body -> { print(" = "); body.accept(this); });
  }

  @Override
  public void visit(final FunctionExpression tree) {
    print('{');
    printFlags(tree);
    if(!tree.typeParameters.isEmpty()) {
      print('[');
      printAll(tree.typeParameters, kCommaSpace);
      print(']');
    }
    if(!tree.isIdempotent()) {
      printAll(tree.parameters, kCommaSpace);
    }
    tree.type.ifPresent(type -> { print(": "); type.accept(this); });
    print(" → ");
    tree.expression.accept(this);
    print('}');
  }

  @Override
  public void visit(final Identifier tree) {
    print(tree.value);
  }

  @Override
  public void visit(final If tree) {
    print("if ");
    if(tree.cases.size() == 1) {
      tree.cases.get(0).accept(this);
    } else {
      println('{');
      pushIndent();
      tree.cases.forEach(ifCase -> { ifCase.accept(this); printNewLine(); });
      popIndent();
      print('}');
    }
  }

  @Override
  public void visit(final IfCase tree) {
    tree.condition.accept(this);
    print(" → ");
    tree.expression.accept(this);
  }

  @Override
  public void visit(final LiteralFalse tree) {
    print("no");
  }

  @Override
  public void visit(LiteralIntegral tree) {
    print(Long.toString(tree.value));
    switch(tree.numericKind) {
      case LiteralNumeric.kInt8:
        print('b');
        break;
      case LiteralNumeric.kInt16:
        print('s');
        break;
      case LiteralNumeric.kInt32:
        break;
      case LiteralNumeric.kInt64:
        print('l');
        break;
      case LiteralNumeric.kUInt8:
        print("ub");
        break;
      case LiteralNumeric.kUInt16:
        print("us");
        break;
      case LiteralNumeric.kUInt32:
        print('u');
        break;
      case LiteralNumeric.kUInt64:
        print("ul");
        break;
    }
  }

  @Override
  public void visit(final LiteralReal tree) {
    print(Double.toString(tree.value));
    if(tree.numericKind == LiteralNumeric.kFloat32) {
      print('f');
    }
  }

  @Override
  public void visit(final LiteralString tree) {
    print('"');
    print(tree.value);
    print('"');
  }

  @Override
  public void visit(final LiteralTrue tree) {
    print("yes");
  }

  @Override
  public void visit(final Module tree) {
    print("module ");
    print(tree.name);
    println(" {");
    pushIndent();
    tree.dependencies.forEach(dependency -> {
      dependency.accept(this);
      printNewLine();
    });
    if(!tree.dependencies.isEmpty()) {
      printNewLine();
    }
    tree.declarations.forEach(declaration -> {
      declaration.accept(this);
      printNewLine();
      //TODO(joa): only if not last
      printNewLine();
    });
    popIndent();
    print('}');
  }

  @Override
  public void visit(final ModuleDependency tree) {
    //TODO(joa): implement me
  }

  @Override
  public void visit(final Parameter tree) {
    print(tree.name);
    tree.type.ifPresent(type -> { print(": "); type.accept(this); });
  }

  @Override
  public void visit(final Program tree) {
    //TODO(joa): new lines
    tree.modules.forEach(module -> module.accept(this));
  }

  @Override
  public void visit(final Select tree) {
    tree.expression.accept(this);
    print('.');
    print(tree.name);
  }

  @Override
  public void visit(final This tree) {
    print("this");
  }

  @Override
  public void visit(final TypeParameter tree) {
    if(tree.variance == TypeParameter.kCovariant) {
      print('+');
    } else if(tree.variance == TypeParameter.kContravariant) {
      print('-');
    }

    print(tree.name);

    tree.bound.ifPresent(bound -> {
      if(tree.boundType == TypeParameter.kBoundExtends) {
        print(" → ");
      } else if (tree.boundType == TypeParameter.kBoundBase) {
        print(" ← ");
      } else {
        print(" <ERROR> ");
      }
      bound.accept(this);
    });
  }

  @Override
  public void visit(TypeReference tree) {
    if(tree.isImmutable()) {
      print("immutable ");
    }

    if(tree.isPure()) {
      print("pure");
    }

    if(!tree.parameters.isEmpty()) {
      if(tree.name == names.predef_function(tree.parameters.size() - 1)) {
        if(tree.parameters.size() == 1) {
          print("→ ");
          tree.parameters.get(0).accept(this);
        } else if(tree.parameters.size() == 2) {
          tree.parameters.get(0).accept(this);
          print(" → ");
          tree.parameters.get(1).accept(this);
        } else {
          int n = tree.parameters.size() - 1;
          final Iterator<Tree> iterator = tree.parameters.iterator();
          print('(');
          iterator.next().accept(this);
          for(int i = 1; i < n && iterator.hasNext(); ++i) {
            print(kCommaSpace);
            iterator.next().accept(this);
          }
          print(") → ");
          iterator.next().accept(this);
        }
        return;
      } else if(tree.name == names.predef_tuple(tree.parameters.size())) {
        print('(');
        printAll(tree.parameters, kCommaSpace);
        print(')');
        return;
      }
    }

    print(tree.name);
    if(!tree.parameters.isEmpty()) {
      print('[');
      printAll(tree.parameters, kCommaSpace);
      print(']');
    }
  }

  @Override
  public void visit(final Variable tree) {
    printVisibility(tree);
    printFlags(tree);
    if(tree.isImmutable()) {
      print("val ");
    } else {
      print("var ");
    }

    print(tree.name);
    tree.type.ifPresent(type -> { print(": "); type.accept(this); });
    tree.initializer.ifPresent(init -> { print(" = "); init.accept(this); });
  }

  @Override
  public void visit(New tree) {
    print("new ");
    tree.klass.accept(this);
    print('(');
    printAll(tree.arguments, kCommaSpace);
    print(')');
  }
}
