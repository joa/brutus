package brutus.compiler.phase;

import brutus.compiler.CompilationUnit;
import brutus.compiler.Context;

/**
 *
 */
public abstract class Phase {
  protected final Context context;

  protected Phase(final Context context) {
    this.context = context;
  }

  public abstract void apply(final CompilationUnit unit);

  public String name() {
    return getClass().getSimpleName();
  }
}
