package brutus.compiler.tree;

import brutus.compiler.scope.Scope;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;
import brutus.compiler.util.Preconditions;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class Block extends Expression {
  public final List<Tree> expressions = new ArrayList<>();

  public Scope scope;

  private Type type;

  public boolean isEmpty() {
    return expressions.isEmpty();
  }

  public Tree lastExpression() {
    return expressions.get(expressions.size() - 1);
  }

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
    return Preconditions.checkNotNull(type);
  }
}
