package brutus.compiler.tree;

/**
 *
 */
public final class LiteralTrue extends Literal {
  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kLiteralTrue;
  }
}
