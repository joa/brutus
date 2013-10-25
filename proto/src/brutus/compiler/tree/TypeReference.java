package brutus.compiler.tree;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;
import brutus.compiler.util.Preconditions;

import java.util.List;

/**
 *
 */
public final class TypeReference extends Expression {
  public final Name name;
  public final List<Tree> parameters;

  private Symbol symbol;

  public TypeReference(final Name name, final List<Tree> parameters) {
    this.name = name;
    this.parameters = parameters;
  }

  @Override
  public void accept(TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kTypeReference;
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
