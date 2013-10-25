package brutus.compiler.report;

import brutus.compiler.name.Name;
import brutus.compiler.tree.Tree;

/**
 *
 */
public final class NoSuchSymbolReport extends Report {
  public final Name name;

  NoSuchSymbolReport(final Name name, final Tree tree) {
    super(tree.position);
    this.name = name;
  }

  @Override
  public int level() {
    return ReportLevel.kError;
  }

  @Override
  public String toString() {
    return "No such symbol: "+name.toString();
  }
}
