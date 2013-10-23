package brutus.compiler.tree;

import brutus.compiler.name.Name;

import java.util.Optional;

/**
 *
 */
public final class ModuleDependency extends Declaration {
  public final Optional<Tree> version;

  public ModuleDependency(final Name name, final Optional<Tree> version) {
    super(name);
    this.version = version;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kModuleDependency;
  }
}
