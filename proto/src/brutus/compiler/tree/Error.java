package brutus.compiler.tree;

/**
 *
 */
public final class Error extends Tree {
  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kError;
  }

  @Override
  public boolean isError() {
    return true;
  }

  //TODO(joa): ErrorSymbol
  //TODO(joa): ErrorType
}
