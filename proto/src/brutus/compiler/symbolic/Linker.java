package brutus.compiler.symbolic;

import brutus.compiler.Context;
import brutus.compiler.scope.Scope;
import brutus.compiler.symbol.Symbol;
import brutus.compiler.tree.*;
import brutus.compiler.tree.Class;

import static brutus.compiler.tree.TreeKind.*;

/**
 *
 */
public final class Linker {
  private final Context context;

  public Linker(final Context context) {
    this.context = context;
  }

  public void link(final Tree tree, final Scope scope, final Symbol owner) {
    switch(tree.kind()) {
      case kArgument: {
        final Argument argument = (Argument)tree;
        link(argument.expression, scope, owner);
        break;
      }

      case kAssign: {
        final Assign assign = (Assign)tree;
        link(assign.lhs, scope, owner);
        link(assign.rhs, scope, owner);
        break;
      }

      case kBlock: {
        final Block block = (Block)tree;
        link(block.expressions, block.scope, owner);
        break;
      }

      case kCall: {
        final Call call = (Call)tree;
        link(call.callee, scope, owner);
        link(call.arguments, scope, owner);
        link(call.typeArguments, scope, owner);
        if(call.callee.symbol().isOverload()) {
          //TODO(joa): resolve
        }
        break;
      }

      case kClass: {
        final Class klass = (Class)tree;
        link(klass.bases, scope, owner);
        link(klass.members, klass.symbol().scope(), klass.symbol());
        break;
      }

      case kFunctionDeclaration: {
        final FunctionDeclaration function = (FunctionDeclaration)tree;
        function.expression.ifPresent(exp -> link(exp, function.symbol().scope(), function.symbol()));
        if(function.type.isPresent()) {
          link(function.type.get(), scope, owner);
        } else {
          //TODO(joa): link type of overridden method
        }
        break;
      }

      case kFunctionExpression: {
        final FunctionExpression function = (FunctionExpression)tree;
        link(function.expression, function.symbol().scope(), function.symbol());
        if(function.type.isPresent()) {
          link(function.type.get(), scope, owner);
        } else {
          //TODO(joa): infer type
        }
        break;
      }

      case kIdentifier:
        break;

      case kIf: {
        final If iif = (If)tree;
        link(iif.cases, iif.scope, owner);
        break;
      }

      case kIfCase: {
        final IfCase ifCase = (IfCase)tree;
        link(ifCase.condition, ifCase.scope, owner);
        link(ifCase.expression, ifCase.scope, owner);
        break;
      }

      case kLiteralFalse: {
        final LiteralFalse literal = (LiteralFalse)tree;
        literal.symbol(context.symbols.predef_brutus_Boolean());
        break;
      }

      case kLiteralTrue: {
        final LiteralTrue literal = (LiteralTrue)tree;
        literal.symbol(context.symbols.predef_brutus_Boolean());
        break;
      }

      case kLiteralString: {
        final LiteralString literal = (LiteralString)tree;
        literal.symbol(context.symbols.predef_brutus_String());
        break;
      }

      case kLiteralReal:
      case kLiteralIntegral: {
        final LiteralNumeric numeric = (LiteralNumeric)tree;
        numeric.symbol(context.symbols.predef_brutus_numeric(numeric.numericKind));
        break;
      }

      case kModule: {
        final Module module = (Module)tree;
        link(module.dependencies, module.symbol().scope(), module.symbol());
        link(module.declarations, module.symbol().scope(), module.symbol());
        break;
      }

      case kModuleDependency: {
        final ModuleDependency moduleDependency = (ModuleDependency)tree;
        break;
      }

      case kNew: {
        final New neu = (New)tree;
        link(neu.klass, scope, owner);
        link(neu.arguments, scope, owner);
        break;
      }

      case kParameter: {
        final Parameter parameter = (Parameter)tree;
        parameter.defaultValue.ifPresent(defaultValue -> link(defaultValue, scope, owner));
        parameter.type.ifPresent(type -> link(type, scope, owner));
        break;
      }

      case kProgram: {
        final Program program = (Program)tree;
        link(program.modules, scope, owner);
        break;
      }

      case kSelect: {
        final Select select = (Select)tree;
        link(select.expression, scope, owner);
        final Symbol selected = select.expression.symbol();
        final Symbol symbol = selected.scope().get(select.name);
        select.symbol(symbol);
        break;
      }

      case kThis: {
        final This thiz = (This)tree;
        Symbol current = owner;

        while(current != null) {
          if(current.kind() == Symbol.kClass) {
            thiz.symbol(current);
            break;
          }

          current = current.parent();
        }

        break;
      }

      case kTypeParameter: {
        final TypeParameter typeParameter = (TypeParameter)tree;
        typeParameter.bound.ifPresent(bound -> link(bound, scope, owner));
        break;
      }

      case kTypeReference: {
        final TypeReference typeReference = (TypeReference)tree;
        link(typeReference.parameters, scope, owner);

        if(typeReference.name.contains('.')) {
          typeReference.symbol(context.symbols.get(typeReference.name));
        } else {
          if(scope.contains(typeReference.name)) {
            typeReference.symbol(scope.get(typeReference.name));
          } else {
            //search in dependent modules
          }
        }
        break;
      }

      case kVariable: {
        final Variable variable = (Variable)tree;
        variable.type.ifPresent(type -> link(type, scope, owner));
        variable.initializer.ifPresent(init -> link(init, scope, owner));
        break;
      }
    }
  }

  private void link(final Iterable<Tree> trees, final Scope scope, final Symbol owner) {
    for(final Tree tree : trees) {
      link(tree, scope, owner);
    }
  }
}
