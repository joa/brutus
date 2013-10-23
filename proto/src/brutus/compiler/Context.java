package brutus.compiler;

import brutus.compiler.name.NameTable;
import brutus.compiler.report.Reporter;
import brutus.compiler.symbol.SymbolTable;

/**
 *
 */
public final class Context {
  public final NameTable names = new NameTable();
  public final SymbolTable symbols = new SymbolTable(names);
  public final Reporter reporter = new Reporter();
}
