package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.tree.Tree;

/**
 *
 */
public final class VariableSymbol extends Symbol {
  private final Tree tree;

  public VariableSymbol(final Name name, final Symbol parent, final Tree tree) {
    super(name, parent);
    this.tree = tree;
    tree.symbol(this);
  }

  @Override
  public int kind() {
    return SymbolKind.kVariable;
  }

  @Override
  public Tree tree() {
    return tree;
  }

  @Override
  public Symbol result() {
    return type().symbol();
  }
}
