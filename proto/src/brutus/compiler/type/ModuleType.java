package brutus.compiler.type;

import brutus.compiler.symbol.Symbol;

/**
 *
 */
public final class ModuleType extends Type {
  public ModuleType(final Symbol symbol) {
    super(symbol);
  }

  @Override
  public int rank() {
    throw new UnsupportedOperationException("ModuleType#rank");
  }
}
