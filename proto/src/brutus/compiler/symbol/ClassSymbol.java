package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.scope.ClassScope;
import brutus.compiler.scope.Scope;
import brutus.compiler.tree.*;
import brutus.compiler.tree.Class;
import brutus.compiler.type.ClassType;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 *
 */
public final class ClassSymbol extends Symbol {
  private final Scope scope;
  private final Tree tree;
  private final Set<Symbol> bases;
  private final List<Symbol> typeParameters;
  private final List<Symbol> members;

  public ClassSymbol(
      final Name name,
      final Symbol parent,
      final Scope parentScope,
      final SymbolTable table,
      final Tree tree) {
    super(name, parent);
    this.scope = new ClassScope(this, parentScope, table);
    this.tree = tree;
    this.bases = new HashSet<>(tree.<Class>mapInt(klass -> klass.bases.size()));
    this.typeParameters = new ArrayList<>(tree.<Class>mapInt(klass -> klass.typeParameters.size()));
    this.members = new ArrayList<>(tree.<Class>mapInt(klass -> klass.members.size()));
    tree.symbol(this);
    type(new ClassType(this));
  }

  @Override
  public Set<Symbol> bases() {
    return bases;
  }

  @Override
  public int kind() {
    return SymbolKind.kClass;
  }

  @Override
  public Scope scope() {
    return scope;
  }

  @Override
  public Tree tree() {
    return tree;
  }

  @Override
  public List<Symbol> members() {
    return members;
  }

  @Override
  public List<Symbol> typeParameters() {
    return typeParameters;
  }
}
