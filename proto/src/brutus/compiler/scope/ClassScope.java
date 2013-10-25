package brutus.compiler.scope;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.*;

import java.util.HashMap;
import java.util.Map;

/**
 *
 */
public class ClassScope extends Scope {
  private final ClassSymbol symbol;

  public ClassScope(ClassSymbol symbol, Scope parent, final SymbolTable table) {
    super(Scope.kClass, parent, table);
    this.symbol = symbol;
  }

  public Symbol get(final Name name) {
    final Symbol result = super.get(name);

    if(!result.isError()) {
      return result;
    }

    for(Symbol base : symbol.bases()) {
      if(base.isError()) {
        continue;
      }

      final Symbol candidate = base.scope().get(name);

      if(!candidate.isError()) {
        return candidate;
      }
    }

    //result contains an error
    return result;
  }

  @Override
  public boolean contains(final Name name) {
    if(super.contains(name)) {
      return true;
    }

    for(final Symbol base : symbol.bases()) {
      if(base.scope().contains(name)) {
        return true;
      }
    }

    return false;
  }
}
