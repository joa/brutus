package brutus.compiler.name;

import brutus.compiler.Constants;
import brutus.compiler.tree.LiteralNumeric;
import brutus.compiler.util.Characters;

/**
 *
 */
public final class NameTable {
  private static final int kDefaultTableSize = 256;
  private static final float kDefaultLoadFactor = 0.75f;
  private static final int kMaximumCapacity = 0xffffffff;

  private Name[] table;
  private int tableSize;
  private final float loadFactor;
  private int size;
  private int threshold;

  public final Name predef_empty;
  public final Name predef_in;
  public final Name predef_out;
  public final Name predef_brutus_Atom;
  public final Name predef_brutus_Boolean;
  public final Name predef_brutus_String;

  private final Name[] predef_function;
  private final Name[] predef_tuple;
  private final Name[] predef_numeric;

  public NameTable() {
    size = 0;
    tableSize = kDefaultTableSize;
    table = new Name[tableSize];
    loadFactor = kDefaultLoadFactor;
    threshold = (int)(loadFactor * tableSize);

    predef_empty = get(new char[0], 0, 0, /*copy=*/false);
    predef_function = new Name[Constants.kMaxFunctionArity];
    predef_tuple = new Name[Constants.kMaxTupleArity];
    predef_in = define("in");
    predef_out = define("out");
    predef_brutus_Atom = define("brutus.Atom");
    predef_brutus_Boolean = define("brutus.Boolean");
    predef_brutus_String = define("brutus.String");
    predef_numeric = new Name[] {
        define("brutus.Int8"),
        define("brutus.Int16"),
        define("brutus.Int32"),
        define("brutus.Int64"),
        define("brutus.UInt8"),
        define("brutus.UInt16"),
        define("brutus.UInt32"),
        define("brutus.UInt64"),
        define("brutus.Float32"),
        define("brutus.Float64")
    };

    for(int i = 2; i < Constants.kMaxTupleArity; ++i) {
      predef_tuple[i] = define("brutus.Tuple"+i);
    }

    for(int i = 0; i < Constants.kMaxFunctionArity; ++i) {
      predef_function[i] = define("brutus.Function"+i);
    }
  }

  private Name define(final String name) {
    return get(name.toCharArray(), 0, name.length(), /*copy=*/false);
  }

  public Name predef_numeric(final int kind) {
    if(kind < LiteralNumeric.kInt8 || kind > LiteralNumeric.kFloat64) {
      return predef_empty;
    }

    return predef_numeric[kind];
  }

  public Name predef_function(final int arity) {
    if(arity >= predef_function.length) {
      return predef_function[predef_function.length - 1];
    }

    return predef_function[arity];
  }

  public Name predef_tuple(final int arity) {
    if(arity < 2) {
      return predef_empty;
    }

    if(arity >= predef_tuple.length) {
      return predef_tuple[predef_tuple.length - 1];
    }

    return predef_tuple[arity];
  }

  public Name get(final char[] chars, final int offset, final int count) {
    return get(chars, offset, count, /*copy=*/false);
  }

  public Name get(final char[] chars, final int offset, final int count, final boolean copy) {
    final int hashCode = rehash(Characters.hashCode(chars, offset, count));
    final int index = indexOf(hashCode, tableSize);
    Name entry = table[index];

    while(entry != null) {
      if(entry.hashCode == hashCode
          && entry.count == count
          && Characters.equal(chars, offset, entry.chars, entry.offset, count)) {
        return entry;
      }

      entry = entry.next;
    }

    // no match found, insert...

    final char[] content;
    final int contentOffset;

    if(copy) {
      content = new char[count];
      contentOffset = 0;
      System.arraycopy(chars, offset, content, 0, count);
    } else {
      content = chars;
      contentOffset = offset;
    }

    final Name oldName = table[index];
    final Name newName = new Name(oldName, hashCode, content, contentOffset, count);

    table[index] = newName;

    if(size++ >= threshold) {
      resize(tableSize << 1);
    }

    return newName;
  }

  private void resize(final int newSize) {
    final Name[] oldTable = table;
    final int oldSize = tableSize;

    if(oldSize == kMaximumCapacity) {
      threshold = Integer.MAX_VALUE;
      return;
    }

    final Name[] newTable = new Name[newSize];
    transfer(oldTable, oldSize, newTable, newSize);

    table = newTable;
    tableSize = newSize;
    threshold = (int)(newSize * loadFactor);
  }

  private void transfer(Name[] src, int srcSize, Name[] dst, int dstSize) {
    for (int i = 0; i < srcSize; i++) {
      Name name = src[i];

      if(name != null) {
        do {
          final Name next = name.next;
          final int index = indexOf(name.hashCode, dstSize);

          name.next = dst[index];
          dst[index] = name;

          name = next;
        } while(name != null);
      }
    }
  }

  private int rehash(final int value) {
    int result = value;
    result ^= (result >> 20) ^ (result >> 12);
    result ^= (result >>  7) ^ (result >>  4);
    return result;
  }

  private int indexOf(final int hashCode, final int length) {
    return hashCode & (length - 1);
  }
}
