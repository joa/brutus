package brutus.compiler.util;

/**
 *
 */
public final class Characters {
  public static boolean equal(final char[] x, final int xOffset, final char[] y, final int yOffset, final int count) {
    final int n = xOffset + count;
    final int m = yOffset + count;

    if(x.length < n) {
      return false;
    }

    if(y.length < m) {
      return false;
    }

    for(int i = xOffset, j = yOffset; i < n && j < m; ++i, ++j) {
      if(x[i] != y[j]) {
        return false;
      }
    }

    return true;
  }

  public static int hashCode(final char[] chars, final int offset, final int count) {
    final int n = offset + count;
    int result = count;

    for(int i = offset; i < n; ++i) {
      result = result * 31 + Character.hashCode(chars[i]);
    }

    return result;
  }
}
