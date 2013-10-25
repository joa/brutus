package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.Scope;
import brutus.compiler.type.ModuleType;

/**
 *
 */
public final class RootSymbol extends Symbol {
  private final Scope scope;

  RootSymbol(final Name name, final SymbolTable symbols) {
    super(name, null);
    scope = new Scope(Scope.kRoot, null, symbols);
    type(new ModuleType(this));
  }

  @Override
  public Scope scope() {
    return scope;
  }

  @Override
  public int kind() {
    return SymbolKind.kRoot;
  }
}
