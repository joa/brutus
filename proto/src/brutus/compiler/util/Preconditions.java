package brutus.compiler.util;

/**
 *
 */
public final class Preconditions {
  public static <T> T checkNotNull(final T value) {
    if(null == value) {
      throw new NullPointerException();
    }

    return value;
  }
  
  private Preconditions() {}
}
