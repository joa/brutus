package brutus.compiler.tree;

/**
 *
 */
public final class LiteralReal extends LiteralNumeric {
  public final double value;

  public LiteralReal(final int numericKind, final double value) {
    super(numericKind);
    this.value = value;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kLiteralReal;
  }
}
