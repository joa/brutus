package brutus.compiler.report;

import brutus.compiler.name.Name;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.tree.Tree;

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

  public void duplicateBase(final Symbol symbol, final Symbol duplicateSymbol) {
    log(new DuplicateBaseReport(symbol, duplicateSymbol));
  }

  public void noSuchSymbol(final Name name, final Tree tree) {
    log(new NoSuchSymbolReport(name, tree));
  }

  public void log(final Report report) {
    incrementTotals(report.level());
    System.out.println("["+report.position().line+":"+report.position().column+"]: "+report.toString());
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
