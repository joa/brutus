package brutus.compiler.tree;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;
import brutus.compiler.util.Preconditions;

/**
 *
 */
public final class Identifier extends Expression {
  public final Name value;

  private Symbol symbol;

  public Identifier(final Name value) {
    this.value = value;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kIdentifier;
  }

  @Override
  public Name name() {
    return value;
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
