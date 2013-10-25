package brutus.compiler.name;

/**
 *
 */
public final class Name {
  Name next = null;
  final int hashCode;

  public final char[] chars;
  public final int offset;
  public final int count;

  Name(final Name next, final int hashCode, char[] chars, final int offset, final int count) {
    this.next = next;
    this.hashCode = hashCode;
    this.chars = chars;
    this.offset = offset;
    this.count = count;
  }

  @Override
  public final String toString() {
    return new String(chars, offset, count);
  }

  public boolean contains(final char c) {
    return indexOf(c, 0) != -1;
  }

  public int indexOf(final char c) {
    return indexOf(c, 0);
  }

  public int indexOf(final char c, final int searchOffset) {
    for(int i = searchOffset, j = offset + searchOffset; i < count; ++i, ++j) {
      if(chars[j] == c) {
        return i;
      }
    }

    return -1;
  }

  public boolean isQualified() {
    return contains('.');
  }
}
