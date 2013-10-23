package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.Scope;
import brutus.compiler.tree.Tree;

/**
 *
 */
public final class ModuleSymbol extends Symbol {
  private final Scope scope;
  private final Tree tree;

  public ModuleSymbol(final Name name, final Symbol parent, final Scope scope, final Tree tree) {
    super(name, parent);
    this.scope = scope;
    this.tree = tree;
    tree.symbol(this);
  }

  @Override
  public int kind() {
    return kModule;
  }

  @Override
  public Tree tree() {
    return tree;
  }

  @Override
  public Scope scope() {
    return scope;
  }
}
