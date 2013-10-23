package brutus.compiler.tree;

/**
 *
 */
public final class LiteralString extends Literal {
  public final String value;

  public LiteralString(final String value) {
    this.value = value;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kLiteralString;
  }
}
