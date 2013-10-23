package brutus.compiler.symbol;

import brutus.compiler.name.Name;

/**
 *
 */
public final class ErrorSymbol extends Symbol {
  public ErrorSymbol(Name name, final Symbol parent) {
    super(name, parent);
  }

  @Override
  public int kind() {
    return Symbol.kError;
  }
}
