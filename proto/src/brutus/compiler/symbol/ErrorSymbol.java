package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.ErrorScope;
import brutus.compiler.scope.Scope;
import brutus.compiler.tree.Tree;
import brutus.compiler.type.ErrorType;

import java.util.HashSet;
import java.util.Set;

/**
 *
 */
public final class ErrorSymbol extends Symbol {
  private final Set<Symbol> bases = new HashSet<>();
  private final Scope scope = new ErrorScope(this);
  private final Tree tree = new brutus.compiler.tree.Error();

  ErrorSymbol(Name name, final Symbol parent) {
    super(name, parent);
    type(new ErrorType(this));
  }

  @Override
  public Set<Symbol> bases() {
    return bases;
  }

  @Override
  public Scope scope() {
    return scope;
  }

  @Override
  public Tree tree() {
    return tree;
  }

  @Override
  public int kind() {
    return SymbolKind.kError;
  }
}
