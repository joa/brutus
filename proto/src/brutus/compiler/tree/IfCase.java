package brutus.compiler.tree;

import brutus.compiler.scope.Scope;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;

/**
 *
 */
public final class IfCase extends Expression {
  public final Tree condition;
  public final Tree expression;

  public Scope scope;

  private Type type;

  public IfCase(final Tree condition, final Tree expression) {
    this.condition = condition;
    this.expression = expression;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kIfCase;
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
