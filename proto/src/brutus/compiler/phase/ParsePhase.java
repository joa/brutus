package brutus.compiler.phase;

import brutus.compiler.CompilationUnit;
import brutus.compiler.Context;
import brutus.compiler.lexical.Parser;
import brutus.compiler.lexical.Scanner;

/**
 *
 */
public final class ParsePhase extends Phase {
  public ParsePhase(Context context) {
    super(context);
  }

  @Override
  public void apply(final CompilationUnit unit) {
    final Scanner scanner = new Scanner(unit.source.charStream());
    final Parser parser = new Parser(unit, scanner, context.names, context.reporter);
    unit.tree = parser.parseProgram();
  }
}
