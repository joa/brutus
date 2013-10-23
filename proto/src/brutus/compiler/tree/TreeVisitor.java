package brutus.compiler.tree;

/**
 *
 */
public interface TreeVisitor {
  public void visit(final Argument tree);
  public void visit(final Assign tree);
  public void visit(final Block tree);
  public void visit(final Call tree);
  public void visit(final Class tree);
  public void visit(final Error tree);
  public void visit(final FunctionDeclaration tree);
  public void visit(final FunctionExpression tree);
  public void visit(final Identifier tree);
  public void visit(final If tree);
  public void visit(final IfCase tree);
  public void visit(final LiteralFalse tree);
  public void visit(final LiteralIntegral tree);
  public void visit(final LiteralReal tree);
  public void visit(final LiteralString tree);
  public void visit(final LiteralTrue tree);
  public void visit(final Module tree);
  public void visit(final ModuleDependency tree);
  public void visit(final Parameter tree);
  public void visit(final Program tree);
  public void visit(final Select tree);
  public void visit(final This tree);
  public void visit(final TypeParameter tree);
  public void visit(final TypeReference tree);
  public void visit(final Variable tree);
  public void visit(final New tree);
}
