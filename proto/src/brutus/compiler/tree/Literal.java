package brutus.compiler.tree;

import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;

/**
 *
 */
public abstract class Literal extends Tree {
  private Symbol symbol;

  @Override
  public Symbol symbol() {
    return symbol;
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
