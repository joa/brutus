package brutus.compiler.tree.util;

import brutus.compiler.name.Name;
import brutus.compiler.name.NameTable;
import brutus.compiler.tree.*;
import brutus.compiler.tree.Class;
import brutus.compiler.tree.Error;
import brutus.compiler.util.CodeWriter;

import java.io.Writer;
import java.lang.reflect.Field;
import java.util.Iterator;
import java.util.List;
import java.util.Optional;

import static brutus.compiler.util.Unchecked.unchecked;

/**
 *
 */
public final class StructurePrinter implements TreeVisitor, CodeWriter.Mixin {
  private final CodeWriter writer;

  public StructurePrinter(final Writer writer) {
    this.writer = new CodeWriter(writer);
  }

  public CodeWriter writer() {
    return writer;
  }

  private void printAll(final Iterable<Tree> iterable) {
    final Iterator<Tree> iter = iterable.iterator();

    if(iter.hasNext()) {
      iter.next().accept(this);
    } else {
      print("<empty>");
    }

    while(iter.hasNext()) {
      println(",");
      iter.next().accept(this);
    }
  }

  private void reflect(final Tree tree) {
    unchecked(() -> {
      final java.lang.Class<? extends Tree> klass = tree.getClass();
      print(klass.getSimpleName());
      print('(');
      pushIndent();
      boolean isFirst = true;
      for(final Field field : klass.getFields()) {
        final java.lang.Class<?> fieldType = field.getType();
        if(List.class.isAssignableFrom(fieldType)) {
          if("java.util.List<brutus.compiler.tree.Tree>".equals(field.getGenericType().getTypeName())) {
            if(!isFirst) {
              println(",");
            }
            isFirst = false;
            @SuppressWarnings("unchecked")
            final List<Tree> trees = (List<Tree>)field.get(tree);
            print(field.getName());

            if(trees.isEmpty()) {
              print(" = []");
            } else {
              println(" = [");
              pushIndent();
              printAll(trees);
              popIndent();
              print("]");
            }
          }
        } else if(Tree.class.isAssignableFrom(fieldType)) {
          if(!isFirst) {
            println(",");
          }
          isFirst = false;
          print(field.getName());
          print(" = ");
          final Tree child = (Tree)field.get(tree);
          child.accept(this);
        } else if(Name.class.isAssignableFrom(fieldType)) {
          if(!isFirst) {
            println(",");
          }
          isFirst = false;
          print(field.getName());
          print(" = ");
          print((Name)field.get(tree));
        } else if(Optional.class.isAssignableFrom(fieldType)) {
          if("java.util.Optional<brutus.compiler.tree.Tree>".equals(field.getGenericType().getTypeName())) {
            if(!isFirst) {
              println(",");
            }
            isFirst = false;
            @SuppressWarnings("unchecked")
            final Optional<Tree> optional = (Optional<Tree>)field.get(tree);
            print(field.getName());
            print(" = ");
            if(optional.isPresent()) {
              optional.get().accept(this);
            } else {
              print("<empty>");
            }
          } else if("java.util.Optional<brutus.compiler.name.Name>".equals(field.getGenericType().getTypeName())) {
            if(!isFirst) {
              println(",");
            }
            isFirst = false;
            @SuppressWarnings("unchecked")
            final Optional<Name> optional = (Optional<Name>)field.get(tree);
            print(field.getName());
            print(" = ");
            if(optional.isPresent()) {
              print(optional.get());
            } else {
              print("<empty>");
            }
          }
        }
      }
      popIndent();
      print(')');
    });
  }

  @Override
  public void visit(final Argument tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Assign tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Block tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Call tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Class tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Error tree) {
    reflect(tree);
  }

  @Override
  public void visit(final FunctionDeclaration tree) {
    reflect(tree);
  }

  @Override
  public void visit(final FunctionExpression tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Identifier tree) {
    reflect(tree);
  }

  @Override
  public void visit(final If tree) {
    reflect(tree);
  }

  @Override
  public void visit(final IfCase tree) {
    reflect(tree);
  }

  @Override
  public void visit(final LiteralFalse tree) {
    reflect(tree);
  }

  @Override
  public void visit(final LiteralIntegral tree) {
    reflect(tree);
  }

  @Override
  public void visit(final LiteralReal tree) {
    reflect(tree);
  }

  @Override
  public void visit(final LiteralString tree) {
    reflect(tree);
  }

  @Override
  public void visit(final LiteralTrue tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Module tree) {
    reflect(tree);
  }

  @Override
  public void visit(final ModuleDependency tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Parameter tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Program tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Select tree) {
    reflect(tree);
  }

  @Override
  public void visit(final This tree) {
    reflect(tree);
  }

  @Override
  public void visit(final TypeParameter tree) {
    reflect(tree);
  }

  @Override
  public void visit(final TypeReference tree) {
    reflect(tree);
  }

  @Override
  public void visit(final Variable tree) {
    reflect(tree);
  }

  @Override
  public void visit(final New tree) {
    reflect(tree);
  }
}
