package brutus.compiler.tree;

/**
 *
 */
public final class LiteralFalse extends Literal {
  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kLiteralFalse;
  }
}
