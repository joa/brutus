package brutus.compiler.tree;

import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;
import brutus.compiler.util.Preconditions;

/**
 *
 */
public final class This extends Expression {
  private Symbol symbol;

  @Override
  public void accept(TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kThis;
  }

  @Override
  public Symbol symbol() {
    return Preconditions.checkNotNull(symbol);
  }

  @Override
  public void symbol(final Symbol value) {
    symbol = value;
  }

  @Override
  public Type type() {
    return symbol.type();
  }
}
