package brutus.compiler;

import brutus.compiler.phase.LinkPhase;
import brutus.compiler.phase.ParsePhase;
import brutus.compiler.phase.Phase;
import brutus.compiler.phase.SymbolPhase;
import brutus.compiler.tree.Tree;
import brutus.compiler.tree.util.StructurePrinter;
import brutus.compiler.tree.util.SyntaxPrinter;
import brutus.compiler.util.Source;

import java.io.PrintWriter;
import java.io.Writer;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static brutus.compiler.util.Unchecked.unchecked;

/**
 *
 */
public final class Compiler {
  public static void main(final String[] args) throws Throwable {
    final Compiler compiler = new Compiler();

    compiler.compile(
        Arrays.asList(
            Source.of(Paths.get("lang.b"))
        )
    );
  }

  private final Context context;

  public Compiler() {
    this(new Context());
  }

  public Compiler(final Context context) {
    this.context = context;
  }

  public void compile(final List<Source> sources) {
    final List<CompilationUnit> units = new ArrayList<>(sources.size());
    final Phase P0 = new ParsePhase(context);
    final Phase P1 = new SymbolPhase(context);
    final Phase P2 = new LinkPhase(context);

    sources.stream().map(CompilationUnit::new).forEach(units::add);
    units.forEach(P0::apply);
    units.forEach(P1::apply);
    units.forEach(P2::apply);
    units.stream().map(unit -> unit.tree).forEach(this::printStructure);
  }

  private void printSyntax(final Tree tree) {
    unchecked(() -> {
      try(final Writer writer = new PrintWriter(System.out, true)) {
        final SyntaxPrinter printer = new SyntaxPrinter(writer, context.names);
        tree.accept(printer);
        writer.flush();
      }
    });
  }


  private void printStructure(final Tree tree) {
    unchecked(() -> {
      try(final Writer writer = new PrintWriter(System.out, true)) {
        final StructurePrinter printer = new StructurePrinter(writer);
        tree.accept(printer);
        writer.flush();
      }
    });
  }
}
