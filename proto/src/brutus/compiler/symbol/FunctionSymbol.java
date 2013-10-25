package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.Scope;
import brutus.compiler.tree.Tree;
import brutus.compiler.type.FunctionType;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 *
 */
public final class FunctionSymbol extends Symbol {
  private final Scope scope;
  private final Tree tree;
  private final Set<Symbol> bases;
  public final List<Symbol> typeParameters = new ArrayList<>();
  public final List<Symbol> parameters = new ArrayList<>();
  public Symbol result;

  public FunctionSymbol(final Name name, final Symbol parent, final Scope scope, final Tree tree) {
    super(name, parent);
    this.scope = scope;
    this.tree = tree;
    this.bases = new HashSet<>(1);
    tree.symbol(this);
    type(new FunctionType(this));
  }

  @Override
  public List<Symbol> typeParameters() {
    return typeParameters;
  }

  @Override
  public List<Symbol> parameters() {
    return parameters;
  }

  @Override
  public Symbol result() {
    return result;
  }

  @Override
  public int kind() {
    return SymbolKind.kFunction;
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
  public Set<Symbol> bases() {
    return bases;
  }

  @Override
  public boolean isOverride() {
    return tree.isOverride();
  }
}
