package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.Scope;

/**
 *
 */
public final class RootSymbol extends Symbol {
  private final Scope scope = new Scope(Scope.kRoot, null);

  RootSymbol(final Name name) {
    super(name, null);
  }

  @Override
  public Scope scope() {
    return scope;
  }

  @Override
  public int kind() {
    return kRoot;
  }
}
