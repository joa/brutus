package brutus.compiler;

import brutus.compiler.tree.Tree;
import brutus.compiler.util.Source;

/**
 *
 */
public final class CompilationUnit {
  public final Source source;
  public Tree tree;

  public CompilationUnit(final Source source) {
    this.source = source;
  }
}
