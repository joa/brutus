package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.Scope;
import brutus.compiler.tree.Tree;
import brutus.compiler.type.Type;

/**
 *
 */
public abstract class Symbol {
  public static final int kError = 1;
  public static final int kOverload = 2;
  public static final int kClass = 3;
  public static final int kFunction = 4;
  public static final int kModule = 5;
  public static final int kTypeParameter = 6;
  public static final int kVariable = 7;
  public static final int kRoot = 8;

  private final Name name;
  private final Symbol parent;

  public Symbol(final Name name, final Symbol parent) {
    this.name = name;
    this.parent = parent;
  }

  public final Name name() {
    return name;
  }

  public final Symbol parent() {
    return parent;
  }

  public final Type type() {
    return null;
  }

  public Scope scope() {
    throw new UnsupportedOperationException("Symbol#scope");
  }

  public Tree tree() {
    throw new UnsupportedOperationException("Symbol#tree");
  }

  public final boolean isClass() {
    return kind() == kClass;
  }

  public final boolean isError() {
    return kind() == kError;
  }

  public final boolean isFunction() {
    return kind() == kFunction;
  }

  public final boolean isModule() {
    return kind() == kModule;
  }

  public final boolean isOverload() {
    return kind() == kOverload;
  }

  public final boolean isRoot() {
    return kind() == kRoot;
  }

  public final boolean isTypeParameter() {
    return kind() == kTypeParameter;
  }

  public final boolean isVariable() {
    return kind() == kVariable;
  }

  public abstract int kind();
}
