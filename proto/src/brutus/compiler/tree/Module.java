package brutus.compiler.tree;

import brutus.compiler.name.Name;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class Module extends Declaration {
  public final List<Tree> dependencies = new ArrayList<>();
  public final List<Tree> declarations = new ArrayList<>();

  public Module(final Name name) {
    super(name);
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kModule;
  }
}
