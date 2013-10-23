package brutus.compiler.tree;

import brutus.compiler.name.Name;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class Class extends Declaration {
  public final List<Tree> members = new ArrayList<>();
  public final List<Tree> typeParameters = new ArrayList<>();
  public final List<Tree> bases = new ArrayList<>();

  public Class(final Name name) {
    super(name);
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kClass;
  }
}
