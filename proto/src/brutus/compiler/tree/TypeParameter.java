package brutus.compiler.tree;

import brutus.compiler.name.Name;

import java.util.Optional;

/**
 *
 */
public final class TypeParameter extends Declaration {
  public static final int kInvariant = 0;
  public static final int kCovariant = 1;
  public static final int kContravariant = 2;

  public static final int kBoundNone = 0;
  public static final int kBoundExtends = 1;
  public static final int kBoundBase = 2;

  public final Optional<Tree> bound;
  public final int boundType;
  public final int variance;

  public TypeParameter(final Name name, final int variance, final Optional<Tree> bound, final int boundType) {
    super(name);
    this.variance = variance;
    this.bound = bound;
    this.boundType = boundType;
  }

  @Override
  public void accept(TreeVisitor visitor) {
    visitor.visit(this);
  }

  @Override
  public int kind() {
    return TreeKind.kTypeParameter;
  }
}
