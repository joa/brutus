package brutus.compiler.tree;

import brutus.compiler.symbol.Symbol;
import brutus.compiler.type.Type;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public final class Call extends Expression {
  public final Tree callee;
  public final List<Tree> arguments = new ArrayList<>();
  public final List<Tree> typeArguments = new ArrayList<>();

  public Call(final Tree callee) {
    this.callee = callee;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kCall;
  }

  @Override
  public Symbol symbol() {
    return callee.symbol();
  }

  @Override
  public Type type() {
    return callee.type();
  }
}
