package brutus.compiler.scope;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.ErrorSymbol;
import brutus.compiler.symbol.OverloadSymbol;
import brutus.compiler.symbol.Symbol;

import java.util.HashMap;
import java.util.Map;

/**
 *
 */
public final class Scope {
  public static final int kUnknown = 0;
  public static final int kBlock = 1;     // block scopes {}
  public static final int kFunction = 2;  // function scope (incl anonymous)
  public static final int kClass = 3;     // class scope
  public static final int kModule = 4;    // module scope
  public static final int kRoot = 5;    // root scope (compiler sym table)

  private final Map<Name, Symbol> symbols = new HashMap<>();

  public final int kind;

  public final Scope parent;

  public Scope(final int kind, final Scope parent) {
    this.kind = kind;
    this.parent = parent;
  }

  public boolean put(final Symbol symbol) {
    return put(symbol.name(), symbol);
  }

  public boolean put(final Name name, final Symbol symbol) {
    if(symbols.containsKey(name)) {
      return false;
    }

    symbols.put(name, symbol);
    return true;
  }

  public boolean putOrOverload(final Name name, final Symbol symbol) {
    final Symbol existing = symbols.get(name);

    if(null != existing) {
      if(existing.kind() == Symbol.kOverload) {
        return ((OverloadSymbol)existing).add(symbol);
      } else {
        final OverloadSymbol overloadSymbol = new OverloadSymbol(existing);
        symbols.put(name, overloadSymbol);
        overloadSymbol.add(symbol);
        return true;
      }
    }

    symbols.put(name, symbol);
    return true;
  }

  public Symbol get(final Name name) {
    final Symbol result = symbols.get(name);

    if(null == result) {
      if(null != parent) {
        return parent.get(name);
      }

      return new ErrorSymbol(name, null);
    }

    return result;
  }

  public boolean contains(final Symbol symbol) {
    return contains(symbol.name());
  }

  public boolean contains(final Name name) {
    return symbols.containsKey(name);
  }
}
