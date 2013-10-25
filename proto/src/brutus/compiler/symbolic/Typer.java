package brutus.compiler.symbolic;

import brutus.compiler.Context;
import brutus.compiler.scope.Scope;
import brutus.compiler.symbol.FunctionSymbol;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.tree.*;
import brutus.compiler.tree.Class;
import brutus.compiler.type.Type;
import brutus.compiler.type.Types;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import static brutus.compiler.tree.TreeKind.*;

/**
 *
 */
public final class Typer {
  private final Context context;

  public Typer(final Context context) {
    this.context = context;
  }

  public void type(final Tree tree, final Scope scope, final Symbol owner) {
    switch(tree.kind()) {
      case kArgument:
        break;

      case kAssign: {
        final Assign assign = (Assign)tree;
        assign.type(unitType());
        break;
      }

      case kBlock:
        final Block block = (Block)tree;
        if(block.isEmpty()) {
          block.type(unitType());
        } else {
          type(block.expressions, block.scope, owner);
          block.type(block.lastExpression().type());
        }
        break;

      case kCall: {
        final Call call = (Call)tree;
        type(call.callee, scope, owner);
        type(call.arguments, scope, owner);
        type(call.typeArguments, scope, owner);
        if(call.callee.symbol().isError()) {
          call.type(context.symbols.error().type());
        } else {
          final FunctionSymbol functionSymbol = (FunctionSymbol)call.callee.symbol();
          call.type(functionSymbol.result.type());
        }
        break;
      }

      case kClass: {
        final Class klass = (Class)tree;
        final Symbol symbol = klass.symbol();
        type(klass.bases, symbol.scope(), symbol);
        type(klass.members, symbol.scope(), symbol);
        type(klass.typeParameters, symbol.scope(), symbol);
        break;
      }

      case kError:
        break;

      case kFunctionDeclaration: {
        final FunctionDeclaration functionDeclaration = (FunctionDeclaration)tree;
        final Symbol symbol = functionDeclaration.symbol();
        type(functionDeclaration.parameters, symbol.scope(), symbol);
        type(functionDeclaration.typeParameters, symbol.scope(), symbol);
        type(functionDeclaration.type, symbol.scope(), symbol);
        type(functionDeclaration.expression, symbol.scope(), symbol);
        break;
      }

      case kFunctionExpression: {
        final FunctionExpression functionExpression = (FunctionExpression)tree;
        final Symbol symbol = functionExpression.symbol();
        type(functionExpression.expression, symbol.scope(), symbol);
        type(functionExpression.parameters, symbol.scope(), symbol);
        type(functionExpression.type, symbol.scope(), symbol);
        type(functionExpression.typeParameters, symbol.scope(), symbol);
        //todo(joa): infer missing type
        break;
      }

      case kIdentifier:
        break;

      case kIf: {
        final If iif = (If)tree;
        final ArrayList<Type> types = new ArrayList<>(iif.cases.size());

        type(iif.cases, iif.scope, owner);

        iif.cases.stream().map(ifCase -> tree.symbol().type()).forEach(types::add);
        iif.type(Types.supremum(types));
      }
    }
  }

  private Type unitType() {
    return context.symbols.predef_brutus_Unit().type();
  }

  private void type(final Optional<Tree> tree, final Scope scope, final Symbol owner) {
    tree.ifPresent(t -> type(t, scope, owner));
  }

  private void type(final List<Tree> trees, final Scope scope, final Symbol owner) {
    for(final Tree tree : trees) {
      type(tree, scope, owner);
    }
  }
}
