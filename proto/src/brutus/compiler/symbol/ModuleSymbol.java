package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.Scope;
import brutus.compiler.tree.Module;
import brutus.compiler.tree.Tree;
import brutus.compiler.type.ModuleType;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class ModuleSymbol extends Symbol {
  private final Scope scope;
  private final Tree tree;
  private final List<Symbol> members;

  public ModuleSymbol(final Name name, final Symbol parent, final Scope scope, final Tree tree) {
    super(name, parent);
    this.scope = scope;
    this.tree = tree;
    this.members = new ArrayList<>(tree.<Module>mapInt(module -> module.declarations.size()));
    tree.symbol(this);
    type(new ModuleType(this));
  }

  @Override
  public List<Symbol> members() {
    return members;
  }

  @Override
  public int kind() {
    return SymbolKind.kModule;
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
