package brutus.compiler.tree;

/**
 *
 */
public final class LiteralIntegral extends LiteralNumeric {
  public final long value;

  public LiteralIntegral(final int numericKind, final long value) {
    super(numericKind);
    this.value = value;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kLiteralIntegral;
  }
}
