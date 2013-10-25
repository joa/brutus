package brutus.compiler.scope;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.Symbol;

/**
 *
 */
public final class ErrorScope extends Scope {
  private final Symbol error;

  public ErrorScope(final Symbol error) {
    super(kError, null, null);
    this.error = error;
  }

  @Override
  public boolean contains(final Name name) {
    return false;
  }

  @Override
  public Symbol get(final Name name) {
    return error;
  }

  @Override
  public boolean put(final Symbol symbol) {
    return false;
  }

  @Override
  public boolean isError() {
    return true;
  }
}
