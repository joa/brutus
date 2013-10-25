package brutus.compiler.type;

import brutus.compiler.symbol.Symbol;

import java.util.Collections;
import java.util.Set;

/**
 *
 */
public final class ErrorType extends Type {
  private final Set<Type> bases = Collections.emptySet();

  public ErrorType(final Symbol error) {
    super(error);
  }

  @Override
  public Set<Type> bases() {
    return bases;
  }

  @Override
  public int rank() {
    return Integer.MAX_VALUE;
  }

  @Override
  public boolean isError() {
    return true;
  }
}
