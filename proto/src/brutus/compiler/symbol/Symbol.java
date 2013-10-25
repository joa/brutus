package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.name.NameTable;
import brutus.compiler.scope.Scope;
import brutus.compiler.tree.Tree;
import brutus.compiler.type.Type;
import brutus.compiler.util.Preconditions;

import java.util.Collections;
import java.util.List;
import java.util.Set;

/**
 *
 */
public abstract class Symbol {
  private final Name name;
  private Name qname;
  private final Symbol parent;
  private Type type;

  public Symbol(final Name name, final Symbol parent) {
    this.name = name;
    this.parent = parent;
  }

  public final Name name() {
    return Preconditions.checkNotNull(name);
  }

  public final Name qualifiedName(final NameTable names) {
    if(null != qname) {
      return qname;
    }

    // start with -1 so we can ++ for the '.' separator
    int totalCount = -1;
    Symbol symbol = this;

    while(!symbol.isRoot()) {
      totalCount++;
      totalCount += symbol.name.count;
      symbol = symbol.parent;
    }

    final char[] nameChars = new char[totalCount];

    symbol = this;
    while(!symbol.isRoot()) {
      System.arraycopy(symbol.name.chars, symbol.name.offset, nameChars, totalCount - symbol.name.count, symbol.name.count);
      totalCount -= symbol.name.count;
      if(totalCount > 0) {
        totalCount--;
        nameChars[totalCount] = '.';
      }
      symbol = symbol.parent;
    }

    return (qname = names.get(nameChars, 0, nameChars.length, /*copy=*/false));
  }

  public final Symbol parent() {
    return Preconditions.checkNotNull(parent);
  }

  public final Type type() {
    return Preconditions.checkNotNull(type);
  }

  public final void type(final Type value) {
    this.type = value;
  }

  public Scope scope() {
    throw new UnsupportedOperationException("Symbol#scope");
  }

  public Tree tree() {
    throw new UnsupportedOperationException("Symbol#tree");
  }

  public Set<Symbol> bases() {
    return Collections.emptySet();
  }

  public List<Symbol> typeParameters() {
    return Collections.emptyList();
  }

  public List<Symbol> parameters() {
    return Collections.emptyList();
  }

  public List<Symbol> members() {
    return Collections.emptyList();
  }

  public Symbol result() {
    return this;
  }

  public final boolean isClass() {
    return kind() == SymbolKind.kClass;
  }

  public final boolean isError() {
    return kind() == SymbolKind.kError;
  }

  public final boolean isFunction() {
    return kind() == SymbolKind.kFunction;
  }

  public final boolean isModule() {
    return kind() == SymbolKind.kModule;
  }

  public final boolean isOverload() {
    return kind() == SymbolKind.kOverload;
  }

  public final boolean isRoot() {
    return kind() == SymbolKind.kRoot;
  }

  public final boolean isAmbiguous() {
    return kind() == SymbolKind.kAmbiguous;
  }

  public final boolean isTypeParameter() {
    return kind() == SymbolKind.kTypeParameter;
  }

  public final boolean isVariable() {
    return kind() == SymbolKind.kVariable;
  }

  public boolean isOverride() {
    return false;
  }

  public boolean isImmutable() {
    return false;
  }

  public boolean isSealed() {
    return false;
  }

  public boolean isVirtual() {
    return false;
  }

  public boolean isPure() {
    return false;
  }

  public boolean isIdempotent() {
    return false;
  }

  public abstract int kind();

  public final Symbol enclosingClass() {
    Symbol symbol = this;

    while(symbol != null && !symbol.isClass()) {
      symbol = symbol.parent();
    }

    return null == symbol ? new ErrorSymbol(name, this) : symbol;
  }
}
