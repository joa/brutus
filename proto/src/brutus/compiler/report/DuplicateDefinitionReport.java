package brutus.compiler.report;

import brutus.compiler.symbol.Symbol;

/**
 *
 */
public final class DuplicateDefinitionReport extends Report {
  public final Symbol existingSymbol;
  public final Symbol duplicateSymbol;

  DuplicateDefinitionReport(final Symbol existing, final Symbol duplicate) {
    super(duplicate.tree().position);
    this.existingSymbol = existing;
    this.duplicateSymbol = duplicate;
  }

  @Override
  public int level() {
    return ReportLevel.kError;
  }
}
