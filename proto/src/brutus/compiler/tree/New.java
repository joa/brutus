package brutus.compiler.tree;

import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class New extends Expression {
  public final Tree klass;
  public final List<Tree> arguments = new ArrayList<>();

  public New(final Tree klass) {
    this.klass = klass;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kNew;
  }

  @Override
  public Symbol symbol() {
    return klass.symbol();
  }

  @Override
  public Type type() {
    return klass.type();
  }
}
