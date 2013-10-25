package brutus.compiler.symbolic;

import brutus.compiler.Context;
import brutus.compiler.name.Name;
import brutus.compiler.scope.Scope;
import brutus.compiler.symbol.*;
import brutus.compiler.tree.*;
import brutus.compiler.tree.Class;
import brutus.compiler.type.Type;

import java.util.ArrayList;
import java.util.List;

import static brutus.compiler.tree.TreeKind.*;

/**
 *
 */
public final class Linker {
  private final Context context;

  public Linker(final Context context) {
    this.context = context;
  }


  public void linkDeclarations(final Tree tree, final Scope scope, final Symbol owner) {
    switch(tree.kind()) {
      case kArgument:
      case kAssign:
      case kBlock:
      case kCall:
        break;
      case kClass: {
        final Class klass = (Class)tree;
        linkDeclarations(klass.bases, scope, owner);
        if(!klass.symbol().isError()) {
          final Symbol symbol = klass.symbol();
          for(final Tree base : klass.bases) {
            if(base.kind() == kTypeReference) {
              final TypeReference typeReference = (TypeReference)base;
              if(symbol.bases().contains(typeReference.symbol())) {
                context.reporter.duplicateBase(symbol, typeReference.symbol());
              } else {
                symbol.bases().add(typeReference.symbol());
                symbol.type().bases().add(typeReference.symbol().type());
              }
            }
          }

          if(klass.bases.isEmpty()) {
            final Name qname = symbol.qualifiedName(context.names);
            if(qname != context.names.predef_brutus_Atom &&
                qname != context.names.predef_brutus_Object &&
                qname != context.names.predef_brutus_Value) {
              symbol.bases().add(context.symbols.predef_brutus_Object());
              symbol.type().bases().add(context.symbols.predef_brutus_Object().type());
            }
          }
        }
        linkDeclarations(klass.members, klass.symbol().scope(), klass.symbol());
        break;
      }

      case kFunctionDeclaration: {
        final FunctionDeclaration function = (FunctionDeclaration)tree;
        if(!function.symbol().isError()) {
          final FunctionSymbol symbol = (FunctionSymbol)function.symbol();
          symbol.bases().add(context.symbols.predef_brutus_function(function.parameters.size()));

          linkDeclarations(function.typeParameters, symbol.scope(), symbol);
          linkDeclarations(function.parameters, symbol.scope(), symbol);

          if(function.type.isPresent()) {
            linkDeclarations(function.type.get(), symbol.scope(), symbol);
          } else {
            //TODO(joa): function MUST be marked override

            if(!symbol.isOverride()) {
              //TODO(joa): add override flag
            }

            //find overridden method
            if(!symbol.parent().isError()) {
              if(symbol.parent().isClass()) {
                final ClassSymbol klass = (ClassSymbol)symbol.parent();

                for(final Type type : klass.type().closure()) {
                  if(type.symbol() == klass) {
                    continue;
                  }

                  if(type.symbol().scope().contains(symbol.name())) {
                    final Symbol baseSymbol = type.symbol().scope().get(symbol.name());
                    //TODO(joa): visibility
                    if(baseSymbol.isFunction()) {
                      final FunctionSymbol baseFunction = (FunctionSymbol)baseSymbol;
                      if(null != baseFunction.result) {
                        symbol.result = baseFunction.result;
                        break;
                      }
                    }
                  }
                }
              } else {
                //TODO(joa): override MUST appear in class body
                System.err.println("Error: 'override' may appear only in class");
              }
            }
          }

          function.type.ifPresent(type -> symbol.result = type.symbol());
        }
        break;
      }

      case kFunctionExpression:
      case kIdentifier:
      case kIf:
      case kIfCase:
      case kLiteralFalse:
      case kLiteralTrue:
      case kLiteralString:
      case kLiteralReal:
      case kLiteralIntegral:
        break;

      case kModule: {
        final Module module = (Module)tree;
        linkDeclarations(module.dependencies, module.symbol().scope(), module.symbol());
        linkDeclarations(module.declarations, module.symbol().scope(), module.symbol());
        break;
      }

      case kModuleDependency: {
        final ModuleDependency moduleDependency = (ModuleDependency)tree;
        break;
      }

      case kNew:
        break;

      case kParameter: {
        final Parameter parameter = (Parameter)tree;
        parameter.defaultValue.ifPresent(defaultValue -> linkDeclarations(defaultValue, scope, owner));
        parameter.type.ifPresent(type -> linkDeclarations(type, scope, owner));
        if(!parameter.symbol().isError()) {
          final VariableSymbol symbol = (VariableSymbol)parameter.symbol();
          parameter.type.ifPresent(type -> symbol.type(Symbols.resolve(type.symbol(), SymbolKind.kClass | SymbolKind.kTypeParameter, context).type()));
        }
        break;
      }

      case kProgram: {
        final Program program = (Program)tree;
        linkDeclarations(program.modules, scope, owner);
        break;
      }

      case kSelect:
      case kThis:
        break;

      case kTypeParameter: {
        final TypeParameter typeParameter = (TypeParameter)tree;
        typeParameter.bound.ifPresent(bound -> linkDeclarations(bound, scope, owner));
        break;
      }

      case kTypeReference: {
        final TypeReference typeReference = (TypeReference)tree;
        linkDeclarations(typeReference.parameters, scope, owner);

        if(typeReference.name.isQualified()) {
          final Symbol symbol = context.symbols.get(typeReference.name);

          if(symbol.isError()) {
            context.reporter.noSuchSymbol(typeReference.name, typeReference);
          }

          typeReference.symbol(Symbols.resolve(symbol, SymbolKind.kClass, context));
        } else {
          if(scope.contains(typeReference.name)) {
            final Symbol symbol = scope.get(typeReference.name);

            if(symbol.isError()) {
              context.reporter.noSuchSymbol(typeReference.name, typeReference);
            }

            typeReference.symbol(Symbols.resolve(symbol, SymbolKind.kClass | SymbolKind.kTypeParameter, context));
          } else {
            //search in dependent modules
            context.reporter.noSuchSymbol(typeReference.name, typeReference);
            typeReference.symbol(context.symbols.error());
          }
        }
        break;
      }

      case kVariable: {
        final Variable variable = (Variable)tree;
        variable.initializer.ifPresent(init -> linkDeclarations(init, scope, owner));
        variable.type.ifPresent(type -> linkDeclarations(type, scope, owner));
        if(!variable.symbol().isError()) {
          final VariableSymbol symbol = (VariableSymbol)variable.symbol();
          variable.type.ifPresent(type -> symbol.type(Symbols.resolve(type.symbol(), SymbolKind.kClass | SymbolKind.kTypeParameter, context).type()));
        }
        break;
      }
    }
  }


  public void linkExpressions(final Tree tree, final Scope scope, final Symbol owner) {
    switch(tree.kind()) {
      case kArgument: {
        final Argument argument = (Argument)tree;
        linkExpressions(argument.expression, scope, owner);
        break;
      }

      case kAssign: {
        final Assign assign = (Assign)tree;
        linkExpressions(assign.lhs, scope, owner);
        linkExpressions(assign.rhs, scope, owner);
        break;
      }

      case kBlock: {
        final Block block = (Block)tree;
        linkExpressions(block.expressions, block.scope, owner);
        break;
      }

      case kCall: {
        final Call call = (Call)tree;
        linkExpressions(call.callee, scope, owner);
        linkExpressions(call.typeArguments, scope, owner);
        linkExpressions(call.arguments, scope, owner);
        if(call.callee.symbol().isOverload()) {
          final Select callee = (Select)call.callee;
          final List<Type> typeArgs = new ArrayList<>(call.typeArguments.size());
          final List<Type> arguments = new ArrayList<>(call.arguments.size());
          call.typeArguments.stream().map(Tree::symbol).map(Symbol::type).forEach(typeArgs::add);
          call.arguments.stream().map(Tree::symbol).map(Symbol::type).forEach(arguments::add);

          callee.symbol(Symbols.resolve(callee.symbol(), typeArgs, arguments, SymbolKind.kFunction, context));
        }

        if(!call.callee.symbol().isAmbiguous() && !call.callee.symbol().isError()) {
          call.type(call.callee.type().result());
        }
        break;
      }

      case kClass: {
        final Class klass = (Class)tree;
        linkExpressions(klass.bases, scope, owner);
        linkExpressions(klass.members, klass.symbol().scope(), klass.symbol());
        break;
      }

      case kFunctionDeclaration: {
        final FunctionDeclaration function = (FunctionDeclaration)tree;
        function.expression.ifPresent(exp -> linkExpressions(exp, function.symbol().scope(), function.symbol()));
        if(function.type.isPresent()) {
          linkExpressions(function.type.get(), function.symbol().scope(), function.symbol());
        }
        break;
      }

      case kFunctionExpression: {
        final FunctionExpression function = (FunctionExpression)tree;
        linkExpressions(function.expression, function.symbol().scope(), function.symbol());

        if(!function.symbol().isError()) {
          final FunctionSymbol symbol = (FunctionSymbol)function.symbol();
          symbol.bases().add(context.symbols.predef_brutus_function(function.parameters.size()));

          linkExpressions(function.typeParameters, symbol.scope(), symbol);
          linkExpressions(function.parameters, symbol.scope(), symbol);

          if(function.type.isPresent()) {
            linkExpressions(function.type.get(), symbol.scope(), symbol);
          } else {
            //TODO(joa): infer type
          }

          if(!function.symbol().isError()) {
            function.type.ifPresent(type -> ((FunctionSymbol)function.symbol()).result = type.symbol());
          }
        }
        break;
      }

      case kIdentifier: {
        final Identifier identifier = (Identifier)tree;
        final Symbol symbol = scope.get(identifier.value);

        if(symbol.isError()) {
          context.reporter.noSuchSymbol(identifier.value, identifier);
        }

        identifier.symbol(symbol);
        break;
      }

      case kIf: {
        final If iif = (If)tree;
        linkExpressions(iif.cases, iif.scope, owner);
        break;
      }

      case kIfCase: {
        final IfCase ifCase = (IfCase)tree;
        linkExpressions(ifCase.condition, ifCase.scope, owner);
        linkExpressions(ifCase.expression, ifCase.scope, owner);
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
        linkExpressions(module.dependencies, module.symbol().scope(), module.symbol());
        linkExpressions(module.declarations, module.symbol().scope(), module.symbol());
        break;
      }

      case kModuleDependency: {
        final ModuleDependency moduleDependency = (ModuleDependency)tree;
        break;
      }

      case kNew: {
        final New neu = (New)tree;
        linkExpressions(neu.klass, scope, owner);
        linkExpressions(neu.arguments, scope, owner);
        break;
      }

      case kParameter: {
        final Parameter parameter = (Parameter)tree;
        parameter.defaultValue.ifPresent(defaultValue -> linkExpressions(defaultValue, scope, owner));
        parameter.type.ifPresent(type -> linkExpressions(type, scope, owner));
        break;
      }

      case kProgram: {
        final Program program = (Program)tree;
        linkExpressions(program.modules, scope, owner);
        break;
      }

      case kSelect: {
        final Select select = (Select)tree;
        linkExpressions(select.expression, scope, owner);
        final Symbol selected = select.expression.symbol().type().result().symbol();
        final Symbol symbol = selected.scope().get(select.name);

        if(symbol.isError()) {
          context.reporter.noSuchSymbol(select.name, select);
        }

        select.symbol(symbol);
        break;
      }

      case kThis: {
        final This thi5 = (This)tree;
        thi5.symbol(owner.enclosingClass());
        break;
      }

      case kTypeParameter: {
        final TypeParameter typeParameter = (TypeParameter)tree;
        typeParameter.bound.ifPresent(bound -> linkExpressions(bound, scope, owner));
        break;
      }

      case kTypeReference: {
        final TypeReference typeReference = (TypeReference)tree;

        //TODO(joa): do not do this work twice...
        linkExpressions(typeReference.parameters, scope, owner);

        if(typeReference.name.isQualified()) {
          final Symbol symbol = context.symbols.get(typeReference.name);

          if(symbol.isError()) {
            context.reporter.noSuchSymbol(typeReference.name, typeReference);
          }

          typeReference.symbol(symbol);
        } else {
          if(scope.contains(typeReference.name)) {
            final Symbol symbol = scope.get(typeReference.name);

            if(symbol.isError()) {
              context.reporter.noSuchSymbol(typeReference.name, typeReference);
            }

            typeReference.symbol(symbol);
          } else {
            //search in dependent modules
          }
        }
        break;
      }

      case kVariable: {
        final Variable variable = (Variable)tree;
        variable.type.ifPresent(type -> linkExpressions(type, scope, owner));
        variable.initializer.ifPresent(init -> linkExpressions(init, scope, owner));
        break;
      }
    }
  }

  private void linkExpressions(final Iterable<Tree> trees, final Scope scope, final Symbol owner) {
    for(final Tree tree : trees) {
      linkExpressions(tree, scope, owner);
    }
  }

  private void linkDeclarations(final Iterable<Tree> trees, final Scope scope, final Symbol owner) {
    for(final Tree tree : trees) {
      linkDeclarations(tree, scope, owner);
    }
  }
}
