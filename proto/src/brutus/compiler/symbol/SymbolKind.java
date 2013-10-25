package brutus.compiler.symbol;

/**
 *
 */
public final class SymbolKind {
  public static final int kError         = 1;
  public static final int kRoot          = 1 << 1;
  public static final int kOverload      = 1 << 2;
  public static final int kAmbiguous     = 1 << 3;
  public static final int kModule        = 1 << 4;
  public static final int kClass         = 1 << 5;
  public static final int kFunction      = 1 << 6;
  public static final int kTypeParameter = 1 << 7;
  public static final int kVariable      = 1 << 8;
}
