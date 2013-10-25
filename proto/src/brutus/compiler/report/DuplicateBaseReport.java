package brutus.compiler.report;

import brutus.compiler.symbol.Symbol;
import brutus.compiler.tree.Tree;

/**
 *
 */
public final class DuplicateBaseReport extends Report {
  public final Symbol symbol;
  public final Symbol duplicateBase;

  DuplicateBaseReport(final Symbol symbol, final Symbol duplicateBase) {
    super(duplicateBase.tree().position);
    this.symbol = symbol;
    this.duplicateBase = duplicateBase;
  }

  @Override
  public int level() {
    return ReportLevel.kError;
  }

  @Override
  public String toString() {
    return "Type Error. "+symbol.name()+" already inherits from "+duplicateBase.name();
  }
}
