package brutus.compiler.tree;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;
import brutus.compiler.util.Preconditions;

/**
 *
 */
public final class Select extends Expression {
  public final Tree expression;
  public final Name name;

  private Symbol symbol;

  public Select(final Tree expression, final Name name) {
    this.expression = expression;
    this.name = name;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kSelect;
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
