package brutus.compiler.tree;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class Program extends Tree {
  public final List<Tree> modules = new ArrayList<>();

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kProgram;
  }
}
