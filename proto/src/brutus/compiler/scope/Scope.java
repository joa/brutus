package brutus.compiler.scope;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.*;

import java.util.HashMap;
import java.util.Map;

/**
 *
 */
public class Scope {
  public static final int kUnknown = 0;
  public static final int kBlock = 1;     // block scopes {}
  public static final int kFunction = 2;  // function scope (incl anonymous)
  public static final int kClass = 3;     // class scope
  public static final int kModule = 4;    // module scope
  public static final int kRoot = 5;      // root scope (compiler sym table)
  public static final int kError = 6;

  private final Map<Name, Symbol> symbols = new HashMap<>();

  public final int kind;
  public final Scope parent;
  private final SymbolTable table;

  public Scope(final int kind, final Scope parent, final SymbolTable table) {
    this.kind = kind;
    this.parent = parent;
    this.table = table;
  }

  public boolean put(final Symbol symbol) {
    final Name name = symbol.name();
    final Symbol existing = symbols.get(name);

    if(null != existing) {
      if(existing.kind() == SymbolKind.kOverload) {
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

      return table.error(name);
    }

    return result;
  }

  public boolean contains(final Name name) {
    return symbols.containsKey(name) || (null != parent && parent.contains(name));
  }

  public boolean isError() {
    return false;
  }
}
