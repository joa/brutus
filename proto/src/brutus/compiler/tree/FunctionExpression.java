package brutus.compiler.tree;

import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;
import brutus.compiler.util.Preconditions;

import java.util.List;
import java.util.Optional;

/**
 *
 */
public final class FunctionExpression extends Expression {
  public final Tree expression;
  public final Optional<Tree> type;
  public final List<Tree> parameters;
  public final List<Tree> typeParameters;

  private Symbol symbol;

  public FunctionExpression(final Tree expression, final Optional<Tree> type,
                            List<Tree> parameters, List<Tree> typeParameters) {
    this.expression = expression;
    this.type = type;
    this.parameters = parameters;
    this.typeParameters = typeParameters;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kFunctionExpression;
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
