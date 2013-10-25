package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.Scope;
import brutus.compiler.tree.Tree;

import java.util.List;
import java.util.Set;

/**
 *
 */
public final class AmbiguousSymbol extends Symbol {
  public final List<Symbol> candidates;

  AmbiguousSymbol(Name name, final Symbol parent, final List<Symbol> candidates) {
    super(name, parent);
    this.candidates = candidates;
  }

  @Override
  public int kind() {
    return SymbolKind.kAmbiguous;
  }

  @Override
  public List<Symbol> typeParameters() {
    throw new UnsupportedOperationException("Ambiguous");
  }

  @Override
  public Tree tree() {
    throw new UnsupportedOperationException("Ambiguous");
  }

  @Override
  public Scope scope() {
    throw new UnsupportedOperationException("Ambiguous");
  }

  @Override
  public Symbol result() {
    throw new UnsupportedOperationException("Ambiguous");
  }

  @Override
  public List<Symbol> parameters() {
    throw new UnsupportedOperationException("Ambiguous");
  }

  @Override
  public List<Symbol> members() {
    throw new UnsupportedOperationException("Ambiguous");
  }

  @Override
  public Set<Symbol> bases() {
    throw new UnsupportedOperationException("Ambiguous");
  }
}
