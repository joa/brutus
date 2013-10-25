package brutus.compiler.tree;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;
import brutus.compiler.util.Preconditions;

/**
 *
 */
public abstract class Declaration extends Tree {
  public final Name name;

  private Symbol symbol;

  protected Declaration(final Name name) {
    this.name = name;
  }

  @Override
  public final Name name() {
    return name;
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
