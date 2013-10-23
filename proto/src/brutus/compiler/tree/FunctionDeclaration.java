package brutus.compiler.tree;

import brutus.compiler.name.Name;

import java.util.List;
import java.util.Optional;

/**
 *
 */
public final class FunctionDeclaration extends Declaration {
  public final Optional<Tree> expression;
  public final Optional<Tree> type;
  public final List<Tree> parameters;
  public final List<Tree> typeParameters;

  public FunctionDeclaration(final Name name, final Optional<Tree> expression, final Optional<Tree> type,
                             List<Tree> parameters, List<Tree> typeParameters) {
    super(name);
    this.expression = expression;
    this.type = type;
    this.parameters = parameters;
    this.typeParameters = typeParameters;
  }

  @Override
  public void accept(final TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kFunctionDeclaration;
  }
}
