package brutus.compiler.tree;

import brutus.compiler.name.Name;

import java.util.Optional;

/**
 *
 */
public final class Variable extends Declaration {
  public final Optional<Tree> type;
  public final Optional<Tree> initializer;

  public Variable(final Name name, final Optional<Tree> type, final Optional<Tree> initializer) {
    super(name);
    this.type = type;
    this.initializer = initializer;
  }

  @Override
  public void accept(TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kVariable;
  }
}
