package brutus.compiler.tree;

import brutus.compiler.name.Name;

import java.util.Optional;

/**
 *
 */
public final class Parameter extends Declaration {
  public final Optional<Tree> type;
  public final Optional<Tree> defaultValue;

  public Parameter(final Name name, final Optional<Tree> type, final Optional<Tree> defaultValue) {
    super(name);
    this.type = type;
    this.defaultValue = defaultValue;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kParameter;
  }
}
