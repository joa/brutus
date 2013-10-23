package brutus.compiler.report;

import brutus.compiler.symbol.Symbol;

/**
 *
 */
public final class Reporter {
  public int totalErrors;
  public int totalWarnings;

  public void invalidSyntax(final int tokExpected, final int tokActual, final Position position) {
    invalidSyntax(InvalidSyntaxReport.kExpect, tokExpected, tokActual, position);
  }

  public void invalidSyntax(final int kind, final int tokExpected, final int tokActual, final Position position) {
    log(new InvalidSyntaxReport(kind, tokExpected, tokActual, position));
  }

  public void duplicateDefinition(final Symbol existing, final Symbol duplicate) {
    log(new DuplicateDefinitionReport(existing, duplicate));
  }

  public void log(final Report report) {
    incrementTotals(report.level());
  }

  private void incrementTotals(final int level) {
    switch(level) {
      case ReportLevel.kDebug:
      case ReportLevel.kInfo:
        break;
      case ReportLevel.kWarning:
        totalWarnings++;
        break;
      case ReportLevel.kError:
        totalErrors++;
        break;
    }
  }
}
