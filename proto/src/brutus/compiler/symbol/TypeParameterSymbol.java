package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.tree.Tree;

/**
 *
 */
public final class TypeParameterSymbol extends Symbol {
  private final Tree tree;

  public TypeParameterSymbol(final Name name, final Symbol parent, final Tree tree) {
    super(name, parent);
    this.tree = tree;
    tree.symbol(this);
  }

  @Override
  public int kind() {
    return kTypeParameter;
  }

  @Override
  public Tree tree() {
    return tree;
  }
}
