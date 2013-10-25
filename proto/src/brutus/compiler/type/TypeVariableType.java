package brutus.compiler.type;

import brutus.compiler.symbol.Symbol;

/**
 *
 */
public final class TypeVariableType extends Type {
  // actually "VariableType" would fit the name better but is
  // confusing and could be mistaken as the type of a a
  public TypeVariableType(final Symbol symbol) {
    super(symbol);
  }
}
