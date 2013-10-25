package brutus.compiler.phase;

import brutus.compiler.CompilationUnit;
import brutus.compiler.Context;
import brutus.compiler.symbolic.Linker;

/**
 *
 */
public final class LinkPhase extends Phase {
  private final Linker linker;

  public LinkPhase(final Context context) {
    super(context);
    this.linker = new Linker(context);
  }

  @Override
  public void apply(final CompilationUnit unit) {
    linker.linkDeclarations(unit.tree, context.symbols.root.scope(), context.symbols.root);
    linker.linkExpressions(unit.tree, context.symbols.root.scope(), context.symbols.root);
  }
}
