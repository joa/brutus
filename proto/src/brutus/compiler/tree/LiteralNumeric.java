package brutus.compiler.tree;

/**
 *
 */
public abstract class LiteralNumeric extends Literal {
  public static final int kInt8    = 0;
  public static final int kInt16   = 1;
  public static final int kInt32   = 2;
  public static final int kInt64   = 3;
  public static final int kUInt8   = 4;
  public static final int kUInt16  = 5;
  public static final int kUInt32  = 6;
  public static final int kUInt64  = 7;
  public static final int kFloat32 = 8;
  public static final int kFloat64 = 9;

  public final int numericKind;

  protected LiteralNumeric(final int numericKind) {
    this.numericKind = numericKind;
  }
}
