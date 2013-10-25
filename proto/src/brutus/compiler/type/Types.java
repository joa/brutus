package brutus.compiler.type;

import java.util.*;
import java.util.stream.Stream;

/**
 *
 */
public final class Types {
  public static Type supremum(final List<Type> types) {
    if(types.isEmpty()) {
      return null;
    }

    final List<Set<Type>> closures = new ArrayList<>(types.size());
    types.stream().map(Type::closure).forEach(closures::add);

    return intersectionOf(closures).stream().max((x, y) -> x.rank() - y.rank()).orElse(null);
  }

  public static Set<Type> intersectionOf(final List<Set<Type>> sets) {
    final Iterator<Set<Type>> iterator = sets.iterator();

    if(!iterator.hasNext()) {
      return Collections.emptySet();
    }

    final Set<Type> intersection = new HashSet<>();
    intersection.addAll(iterator.next());

    while(iterator.hasNext()) {
      intersection.retainAll(iterator.next());
    }

    return intersection;
  }

  public static boolean isSubtype(final Type a, final Type b) {
    return a.closure().contains(b);
  }

  public static boolean allSubtype(final List<Type> typesA, final List<Type> typesB) {
    if(typesA.size() != typesB.size()) {
      return false;
    }

    final Iterator<Type> iteratorA = typesA.iterator();
    final Iterator<Type> iteratorB = typesB.iterator();

    while(iteratorA.hasNext()) {
      if(!iteratorB.hasNext()) {
        //better safe than sorry
        return false;
      }

      if(!isSubtype(iteratorA.next(), iteratorB.next())) {
        return false;
      }
    }

    return true;
  }

  public static boolean equal(final Type a, final Type b) {
    return a == b;
  }

  private Types() {}
}
