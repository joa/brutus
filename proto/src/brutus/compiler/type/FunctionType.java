package brutus.compiler.type;

import brutus.compiler.symbol.FunctionSymbol;
import brutus.compiler.util.Preconditions;

import java.util.HashSet;
import java.util.Set;

/**
 *
 */
public final class FunctionType extends Type {
  private final Set<Type> bases;

  public FunctionType(final FunctionSymbol symbol) {
    super(symbol);
    bases = new HashSet<>(1);
  }

  @Override
  public Set<Type> bases() {
    return bases;
  }

  @Override
  public Type result() {
    return Preconditions.checkNotNull(symbol().result()).type();
  }
}
