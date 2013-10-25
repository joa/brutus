package brutus.compiler.symbolic;

import brutus.compiler.Context;
import brutus.compiler.scope.Scope;
import brutus.compiler.symbol.*;
import brutus.compiler.tree.*;
import brutus.compiler.tree.Class;

import static brutus.compiler.tree.TreeKind.*;

/**
 *
 */
public final class Attributer {
  private final Context context;

  public Attributer(final Context context) {
    this.context = context;
  }

  public void attribute(final Tree tree, final Scope scope, final Symbol owner) {
    switch(tree.kind()) {
      case kArgument: {
        final Argument argument = (Argument)tree;
        attribute(argument.expression, scope, owner);
        break;
      }

      case kAssign: {
        final Assign assign = (Assign)tree;
        attribute(assign.lhs, scope, owner);
        attribute(assign.rhs, scope, owner);
        break;
      }

      case kBlock: {
        final Block block = (Block)tree;
        block.scope = new Scope(Scope.kBlock, scope, context.symbols);
        attribute(block.expressions, block.scope, owner);
        break;
      }

      case kCall: {
        final Call call = (Call)tree;
        attribute(call.callee, scope, owner);
        attribute(call.arguments, scope, owner);
        attribute(call.typeArguments, scope, owner);
        break;
      }

      case kClass: {
        final Class klass = (Class)tree;
        final ClassSymbol symbol = new ClassSymbol(klass.name, owner, scope, context.symbols, klass);

        if(!scope.isError() && !scope.put(symbol)) {
          context.reporter.duplicateDefinition(scope.get(symbol.name()), symbol);
        }

        attribute(klass.typeParameters, symbol.scope(), symbol);
        attribute(klass.members, symbol.scope(), symbol);
        klass.typeParameters.stream().map(Tree::symbol).forEach(symbol.typeParameters()::add);
        klass.members.stream().map(Tree::symbol).forEach(symbol.members()::add);
        break;
      }

      case kFunctionDeclaration: {
        final FunctionDeclaration function = (FunctionDeclaration)tree;
        final FunctionSymbol symbol = new FunctionSymbol(function.name, owner, new Scope(Scope.kFunction, scope, context.symbols), function);

        if(!scope.isError() && !scope.put(symbol)) {
          context.reporter.duplicateDefinition(scope.get(symbol.name()), symbol);
        }

        attribute(function.typeParameters, symbol.scope(), symbol);
        attribute(function.parameters, symbol.scope(), symbol);
        function.type.ifPresent(type -> attribute(type, scope, owner));
        function.expression.ifPresent(exp -> attribute(exp, symbol.scope(), symbol));

        function.typeParameters.stream().map(Tree::symbol).forEach(symbol.typeParameters::add);
        function.parameters.stream().map(Tree::symbol).forEach(symbol.parameters::add);
        break;
      }

      case kFunctionExpression: {
        final FunctionExpression function = (FunctionExpression)tree;
        final FunctionSymbol symbol = new FunctionSymbol(context.names.predef_empty, owner, new Scope(Scope.kFunction, scope, context.symbols), function);
        function.type.ifPresent(type -> attribute(type, scope, owner));
        attribute(function.typeParameters, symbol.scope(), symbol);
        attribute(function.parameters, symbol.scope(), symbol);
        attribute(function.expression, symbol.scope(), symbol);
        function.typeParameters.stream().map(Tree::symbol).forEach(symbol.typeParameters::add);
        function.parameters.stream().map(Tree::symbol).forEach(symbol.parameters::add);
        function.type.ifPresent(type -> symbol.result = type.symbol());
        break;
      }

      case kIdentifier:
        break;

      case kIf: {
        final If iff = (If)tree;
        iff.scope = new Scope(Scope.kBlock, scope, context.symbols);
        attribute(iff.cases, iff.scope, owner);
        break;
      }

      case kIfCase: {
        final IfCase ifCase = (IfCase)tree;
        ifCase.scope = new Scope(Scope.kBlock, scope, context.symbols);
        attribute(ifCase.condition, ifCase.scope, owner);
        break;
      }

      case kLiteralFalse:
      case kLiteralTrue:
      case kLiteralIntegral:
      case kLiteralReal:
      case kLiteralString:
        break;

      case kModule: {
        final Module module = (Module)tree;
        final ModuleSymbol symbol = new ModuleSymbol(module.name, owner, new Scope(Scope.kModule, scope, context.symbols), module);
        //dependencies
        scope.put(symbol);
        attribute(module.declarations, symbol.scope(), symbol);
        module.declarations.stream().map(Tree::symbol).forEach(symbol.members()::add);
        break;
      }

      case kModuleDependency:
        break;

      case kNew: {
        final New neu = (New)tree;
        attribute(neu.klass, scope, owner);
        attribute(neu.arguments, scope, owner);
        break;
      }

      case kParameter: {
        final Parameter parameter = (Parameter)tree;
        final VariableSymbol symbol = new VariableSymbol(parameter.name, owner, parameter);
        scope.put(symbol);
        break;
      }

      case kProgram: {
        final Program program = (Program)tree;
        attribute(program.modules, scope, owner);
        break;
      }

      case kSelect: {
        final Select select = (Select)tree;
        attribute(select.expression, scope, owner);
        break;
      }

      case kThis:
        break;

      case kTypeParameter: {
        final TypeParameter typeParameter = (TypeParameter)tree;
        final TypeVariableSymbol symbol = new TypeVariableSymbol(typeParameter.name, owner, typeParameter);
        typeParameter.bound.ifPresent(bound -> attribute(bound, scope, owner));
        scope.put(symbol);
        break;
      }

      case kTypeReference: {
        final TypeReference typeReference = (TypeReference)tree;
        attribute(typeReference.parameters, scope, owner);
        break;
      }

      case kVariable: {
        final Variable variable = (Variable)tree;
        final VariableSymbol symbol = new VariableSymbol(variable.name, owner, variable);
        variable.initializer.ifPresent(init -> attribute(init, scope, owner));
        variable.type.ifPresent(type -> attribute(type, scope, owner));
        scope.put(symbol);
        break;
      }
    }
  }

  private void attribute(final Iterable<Tree> iterable, final Scope scope, final Symbol owner) {
    for(final Tree tree : iterable) {
      attribute(tree, scope, owner);
    }
  }
}
