package brutus.compiler.tree;

import brutus.compiler.name.Name;
import brutus.compiler.report.Position;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;

import java.util.function.Function;

/**
 *
 */
public abstract class Tree {
  public static final int ACC_VIRTUAL = 1;
  public static final int ACC_PUBLIC = 1 << 1;
  public static final int ACC_PRIVATE = 1 << 2;
  public static final int ACC_PROTECTED = 1 << 3;
  public static final int ACC_INTERNAL = 1 << 4;
  public static final int ACC_NATIVE = 1 << 5;
  public static final int ACC_FORCE = 1 << 6;
  public static final int ACC_ABSTRACT = 1 << 7;
  public static final int ACC_PURE = 1 << 8;
  public static final int ACC_IMMUTABLE = 1 << 9;
  public static final int ACC_IDEMPOTENT = 1 << 10;
  public static final int ACC_OVERRIDE = 1 << 11;
  public static final int ACC_SEALED = 1 << 12;
  public static final int ACC_PARTIAL = 1 << 13;

  private int flags = 0;

  public Position position;

  public abstract void accept(final TreeVisitor visitor);

  public abstract int kind();

  public Symbol symbol() {
    throw new UnsupportedOperationException("Tree#symbol");
  }

  public void symbol(final Symbol value) {
    throw new UnsupportedOperationException("Tree#symbol");
  }

  public Type type() {
    throw new UnsupportedOperationException("Tree#type");
  }

  public void type(final Type type) {
    throw new UnsupportedOperationException("Tree#type");
  }

  public Name name() {
    throw new UnsupportedOperationException("Tree#name");
  }

  public final Tree withFlags(final int value) {
    flags = value;
    return this;
  }

  public final boolean isVirtual() {
    return (flags & ACC_VIRTUAL) == ACC_VIRTUAL;
  }

  public final boolean isPublic() {
    return (flags & ACC_PUBLIC) == ACC_PUBLIC;
  }

  public final boolean isPrivate() {
    return (flags & ACC_PRIVATE) == ACC_PRIVATE;
  }

  public final boolean isProtected() {
    return (flags & ACC_PROTECTED) == ACC_PROTECTED;
  }

  public final boolean isInternal() {
    return (flags & ACC_INTERNAL) == ACC_INTERNAL;
  }

  public final boolean isNative() {
    return (flags & ACC_NATIVE) == ACC_NATIVE;
  }

  public final boolean isForced() {
    return (flags & ACC_FORCE) == ACC_FORCE;
  }

  public final boolean isAbstract() {
    return (flags & ACC_ABSTRACT) == ACC_ABSTRACT;
  }

  public final boolean isPure() {
    return (flags & ACC_PURE) == ACC_PURE;
  }

  public final boolean isImmutable() {
    return (flags & ACC_IMMUTABLE) == ACC_IMMUTABLE;
  }

  public final boolean isIdempotent() {
    return (flags & ACC_IDEMPOTENT) == ACC_IDEMPOTENT;
  }

  public final boolean isPartial() {
    return (flags & ACC_PARTIAL) == ACC_PARTIAL;
  }

  public final boolean isSealed() {
    return (flags & ACC_SEALED) == ACC_SEALED;
  }

  public final boolean isOverride() {
    return (flags & ACC_OVERRIDE) == ACC_OVERRIDE;
  }

  public boolean isError() {
    return false;
  }

  public <T extends Tree> int mapInt(final Function<T, Integer> f) {
    if(isError()) {
      return 0;
    }

    @SuppressWarnings("unchecked")
    final T casted = (T)this;

    return f.apply(casted);
  }
}
