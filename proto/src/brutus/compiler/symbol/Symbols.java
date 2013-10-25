package brutus.compiler.symbol;

import brutus.compiler.Context;
import brutus.compiler.type.Type;
import brutus.compiler.type.Types;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class Symbols {
  public static Symbol resolve(final Symbol symbol, final int kinds, final Context context) {
    if(symbol.isAmbiguous() || symbol.isError()) {
      return symbol;
    }

    if((symbol.kind() & kinds) != 0) {
      return symbol;
    }

    if(symbol.isOverload()) {
      final OverloadSymbol overloadSymbol = (OverloadSymbol)symbol;
      final ArrayList<Symbol> candidates = new ArrayList<>(1);

      for(final Symbol alternative : overloadSymbol.alternatives) {
        if(alternative.isAmbiguous() || alternative.isError()) {
          continue;
        }

        if((alternative.kind() & kinds) != 0) {
          candidates.add(alternative);
        }
      }

      if(candidates.size() == 1) {
        return candidates.get(0);
      }

      candidates.trimToSize();
      return new AmbiguousSymbol(overloadSymbol.name(), overloadSymbol.parent(), candidates);
    }

    System.err.println("INTERNAL ERROR");
    return context.symbols.error();
  }

  public static Symbol resolve(final Symbol symbol, final List<Type> typeArguments, final List<Type> arguments, final int kinds, final Context context) {
    if(symbol.isAmbiguous() || symbol.isError()) {
      return symbol;
    }

    if((symbol.kind() & kinds) != 0) {
      return symbol;
    }

    if(symbol.isOverload()) {
      final OverloadSymbol overloadSymbol = (OverloadSymbol)symbol;
      final ArrayList<Symbol> candidates = new ArrayList<>(1);

      for(final Symbol alternative : overloadSymbol.alternatives) {
        if(alternative.isAmbiguous() || alternative.isError()) {
          continue;
        }

        //TODO(joa): this does not respect default/named arguments yet
        if((alternative.kind() & kinds) != 0 &&
            alternative.typeParameters().size() == typeArguments.size() &&
            alternative.parameters().size() == arguments.size()) {
          final List<Type> parameters = new ArrayList<>(alternative.parameters().size());
          alternative.parameters().stream().map(Symbol::type).forEach(parameters::add);
          if(Types.allSubtype(parameters, arguments)) {
            candidates.add(alternative);
          }
        }
      }

      if(candidates.size() == 1) {
        return candidates.get(0);
      }

      candidates.trimToSize();
      return new AmbiguousSymbol(overloadSymbol.name(), overloadSymbol.parent(), candidates);
    }

    System.err.println("INTERNAL ERROR");
    return context.symbols.error();
  }

  private Symbols() {}
}
