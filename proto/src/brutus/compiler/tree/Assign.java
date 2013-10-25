package brutus.compiler.tree;

import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;
import brutus.compiler.util.Preconditions;

/**
 *
 */
public final class Assign extends Expression {
  public final Tree lhs;
  public final Tree rhs;

  //smell: always unit
  private Type type;

  public Assign(final Tree lhs, final Tree rhs) {
    this.lhs = lhs;
    this.rhs = rhs;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kAssign;
  }

  @Override
  public Symbol symbol() {
    return lhs.symbol();
  }

  @Override
  public void type(final Type value) {
    this.type = value;
  }

  @Override
  public Type type() {
    return Preconditions.checkNotNull(type);
  }
}
