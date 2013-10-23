package brutus.compiler.report;

import brutus.compiler.CompilationUnit;

/**
 *
 */
public final class Position {
  public final CompilationUnit unit;
  public final int line;
  public final int column;

  public Position(final CompilationUnit unit, final int line, final int column) {
    this.unit = unit;
    this.line = line;
    this.column = column;
  }
}
