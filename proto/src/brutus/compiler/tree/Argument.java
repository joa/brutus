package brutus.compiler.tree;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;

import java.util.Optional;

/**
 *
 */
public final class Argument extends Expression {
  public final Tree expression;
  public final Optional<Name> name;

  public Argument(final Tree expression, final Optional<Name> name) {
    this.expression = expression;
    this.name = name;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kArgument;
  }

  @Override
  public Symbol symbol() {
    return expression.symbol();
  }

  @Override
  public Type type() {
    return expression.type();
  }
}
