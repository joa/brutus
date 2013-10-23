package brutus.compiler.phase;

import brutus.compiler.CompilationUnit;
import brutus.compiler.Context;
import brutus.compiler.symbolic.Attributer;

/**
 *
 */
public final class SymbolPhase extends Phase {
  private final Attributer attributer;

  public SymbolPhase(final Context context) {
    super(context);
    this.attributer = new Attributer(context);
  }

  @Override
  public void apply(final CompilationUnit unit) {
    attributer.attribute(unit.tree, context.symbols.root.scope(), context.symbols.root);
  }
}
