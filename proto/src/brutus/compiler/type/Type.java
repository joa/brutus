package brutus.compiler.type;

import brutus.compiler.symbol.Symbol;

import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 *
 */
public class Type {
  private final Symbol symbol;
  private final Set<Type> closure = new HashSet<>(2);

  private int rank = -1;

  protected Type(final Symbol symbol) {
    this.symbol = symbol;
  }

  public Type result() {
    return this;
  }

  public final Symbol symbol() {
    return symbol;
  }

  public Set<Type> bases() {
    return Collections.emptySet();
  }

  public final Set<Type> closure() {
    if(closure.isEmpty()) {
      closure.add(this);
      bases().stream().map(Type::closure).forEach(closure::addAll);
    }

    return closure;
  }

  public int rank() {
    if(rank == -1) {
      for(final Type type : bases()) {
        if(type.rank() > rank) {
          rank = type.rank();
        }
      }
      ++rank;
    }

    return rank;
  }

  public boolean isError() {
    return false;
  }
}
