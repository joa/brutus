package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.Scope;
import brutus.compiler.tree.Tree;
import brutus.compiler.type.Types;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

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
    if(alternative.isClass() && alternatives.stream().anyMatch(Symbol::isClass)) {
      return false;
    } else if(alternative.isModule() && alternatives.stream().anyMatch(Symbol::isModule)) {
      return false;
    } else if(alternative.isVariable() && alternatives.stream().anyMatch(Symbol::isVariable)) {
      return false;
    } else if(alternative.isTypeParameter() && alternatives.stream().anyMatch(Symbol::isTypeParameter)) {
      return false;
    } else if(alternative.isFunction() && alternatives.stream().filter(Symbol::isFunction).anyMatch(function -> {
      if(alternative.typeParameters().size() == function.typeParameters().size() &&
          alternative.parameters().size() == function.parameters().size()) {
        // two functions, identical name && identical amount of parameters
        // check if parameter types are equal
        // TODO(joa): this runs in O(n*m)
        for(final Symbol p0 : alternative.parameters()) {
          if(function.parameters().stream().anyMatch(p1 -> Types.equal(p0.type(), p1.type()))) {
            return true; //two identical function declarations
          }
        }
      }
      return false; // this means, function is not identical
    })) {
      return false;
    }

    alternatives.add(alternative);
    return true;
  }

  @Override
  public int kind() {
    return SymbolKind.kOverload;
  }

  @Override
  public List<Symbol> typeParameters() {
    throw new UnsupportedOperationException("Overload");
  }

  @Override
  public Tree tree() {
    throw new UnsupportedOperationException("Overload");
  }

  @Override
  public Scope scope() {
    throw new UnsupportedOperationException("Overload");
  }

  @Override
  public Symbol result() {
    throw new UnsupportedOperationException("Overload");
  }

  @Override
  public List<Symbol> parameters() {
    throw new UnsupportedOperationException("Overload");
  }

  @Override
  public List<Symbol> members() {
    throw new UnsupportedOperationException("Overload");
  }

  @Override
  public Set<Symbol> bases() {
    throw new UnsupportedOperationException("Overload");
  }
}
