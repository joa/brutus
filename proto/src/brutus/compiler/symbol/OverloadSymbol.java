package brutus.compiler.symbol;

import brutus.compiler.name.Name;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class OverloadSymbol extends Symbol {
  public final List<Symbol> alternatives = new ArrayList<>(2);

  public OverloadSymbol(final Symbol alternative) {
    this(alternative.name(), alternative.parent());
    add(alternative);
  }

  public OverloadSymbol(final Name name, final Symbol parent) {
    super(name, parent);
  }

  public boolean add(final Symbol alternative) {
    //TODO(joa): check if alternative already exists, return false
    alternatives.add(alternative);
    return true;
  }

  @Override
  public int kind() {
    return Symbol.kOverload;
  }
}
