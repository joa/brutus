package brutus.compiler.tree;

import brutus.compiler.scope.Scope;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class Block extends Expression {
  public final List<Tree> expressions = new ArrayList<>();

  public Scope scope;

  private Type type;

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kBlock;
  }

  @Override
  public Symbol symbol() {
    return type.symbol();
  }

  @Override
  public void type(final Type value) {
    type = value;
  }

  @Override
  public Type type() {
    return type;
  }
}
