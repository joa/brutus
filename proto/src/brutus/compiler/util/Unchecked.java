package brutus.compiler.util;

/**
 *
 */
public final class Unchecked {
  @FunctionalInterface
  public static interface UncheckedFunction {
    public void apply() throws Throwable;
  }

  public static void unchecked(final UncheckedFunction f) {
    try {
      f.apply();
    } catch(final Throwable throwable) {
      uncheckedThrow(throwable);
    }
  }

  private static void uncheckedThrow(final Throwable t) {
    Unchecked.<RuntimeException>throwWithUncheckedCast(t);
  }

  @SuppressWarnings("unchecked")
  private static <T extends Throwable> void throwWithUncheckedCast(final Throwable t) throws T {
    throw (T)t;
  }
}
