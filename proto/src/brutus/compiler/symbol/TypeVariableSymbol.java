package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.tree.Tree;
import brutus.compiler.type.TypeVariableType;

/**
 *
 */
public final class TypeVariableSymbol extends Symbol {
  private final Tree tree;

  public TypeVariableSymbol(final Name name, final Symbol parent, final Tree tree) {
    super(name, parent);
    this.tree = tree;
    tree.symbol(this);
    type(new TypeVariableType(this));
  }

  @Override
  public int kind() {
    return SymbolKind.kTypeParameter;
  }

  @Override
  public Tree tree() {
    return tree;
  }
}
