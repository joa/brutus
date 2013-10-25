package brutus.compiler.type;

import brutus.compiler.symbol.ClassSymbol;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.tree.*;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 *
 */
public final class ClassType extends Type {
  private final Set<Type> bases;

  public ClassType(final ClassSymbol symbol) {
    super(symbol);
    this.bases = new HashSet<>(symbol.tree().isError() ? 0 : ((brutus.compiler.tree.Class)symbol.tree()).bases.size());
  }

  @Override
  public Set<Type> bases() {
    return bases;
  }

  @Override
  public Type result() {
    return this;
  }
}
