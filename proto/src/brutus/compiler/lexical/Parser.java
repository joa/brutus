package brutus.compiler.lexical;

import brutus.compiler.CompilationUnit;
import brutus.compiler.name.Name;
import brutus.compiler.report.InvalidSyntaxReport;
import brutus.compiler.report.Position;
import brutus.compiler.report.Reporter;
import brutus.compiler.tree.*;
import brutus.compiler.name.NameTable;
import brutus.compiler.tree.Class;
import brutus.compiler.tree.Error;

import static brutus.compiler.lexical.Token.*;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

/**
 *
 */
public final class Parser {
  private final CompilationUnit unit;
  private final Scanner scanner;
  private final NameTable names;
  private int currentToken;
  private final Reporter reporter;

  public Parser(final CompilationUnit unit, final Scanner scanner, final NameTable names, final Reporter reporter) {
    this.unit = unit;
    this.scanner = scanner;
    this.names = names;
    this.currentToken = kError;
    this.reporter = reporter;
  }

  //
  // ArgumentList
  //  : Argument (',' Argument)*
  //
  public void parseArgumentList(final List<Tree> list) {
    do {
      list.add(parseArgument());
    } while(poll(kComma));
  }

  //
  // AnonymousFunctionParameterList
  //  : AnonymousFunctionParameter (',' AnonymousFunctionParameter)*
  //
  public void parseAnonymousFunctionParameterList(final List<Tree> list) {
    do {
      list.add(parseAnonymousFunctionParameter());
    } while(poll(kComma));
  }

  public boolean peekVisibility() {
    return peek(kPublic)
        || peek(kPrivate)
        || peek(kProtected)
        || peek(kInternal);
  }

  public boolean peekDeclarationFlag() {
    return peek(kVirtual)
        || peek(kImmutable)
        || peek(kIdempotent)
        || peek(kSealed)
        || peek(kPure)
        || peek(kPartial)
        || peek(kNative);
  }

  public boolean peekPrimaryExpression() {
    return peek(kIdentifier)
        || peek(kNumberLiteral)
        || peek(kTrue)
        || peek(kYes)
        || peek(kFalse)
        || peek(kNo)
        || peek(kStringLiteral)
        || peek(kThis);
  }

  public void parseParameterList(final List<Tree> list) {
    do {
      list.add(parseParameter());
    } while(poll(kComma));
  }

  //
  // Program
  //  : (Module NEWLINE)*
  //  ;
  //
  public Tree parseProgram() {
    advance();

    final Program program = init(new Program());
    final List<Tree> modules = program.modules;

    while(peek(kModule)) {
      modules.add(parseModule());

      if(!poll(kNewLine)) {
        return expected(kNewLine);
      }
    }

    return program;
  }

  //
  // Module
  //  : 'module' ActorName '{' NEWLINE (ModuleDeclaration NEWLINE)* '}'
  //  ;
  //
  // ModuleDeclaration
  //  : ModuleDependency
  //  | Declaration
  //  | Expression
  //  ;
  //
  public Tree parseModule() {
    if(!poll(kModule)) {
      return expected(kModule);
    }

    final Name name = parseName();

    if(!poll(kLBrace)) {
      return expected(kLBrace);
    }

    if(!poll(kNewLine)) {
      return expected(kNewLine);
    }

    final Module module = init(new Module(name));
    final List<Tree> dependencies = module.dependencies;
    final List<Tree> declarations = module.declarations;

    do {
      // ModuleDeclaration
      if(peek(kRequire)) {
        dependencies.add(parseModuleDependency());
      } else {
        final Tree declaration = parseDeclarationOpt();

        if(declaration == null) {
          declarations.add(parseExpression());
        } else {
          declarations.add(declaration);
        }
      }

      if(!poll(kNewLine)) {
        return expected(kNewLine);
      }
    } while(!peek(kRBrace));

    if(!poll(kRBrace)) {
      return expected(kRBrace);
    }

    return module;
  }

  //
  // ModuleDependency
  //  : 'require' ActorName ModuleVersion?
  //  ;
  //
  public Tree parseModuleDependency() {
    if(!poll(kRequire)) {
      return expected(kRequire);
    }

    final Name name = parseName();
    final Optional<Tree> version;

    if(peek(kColon)) {
      version = Optional.of(parseModuleVersion());
    } else {
      version = Optional.empty();
    }

    return init(new ModuleDependency(name, version));
  }

  //
  // ModuleVersion
  //  : ':' ModuleVersionRange
  //  ;
  //
  // ModuleVersionRange
  //  : ModuleVersionRange (',' ModuleVersionRange)*
  //  | NumberLiteral
  //  | '[' NumberLiteral ']'
  //  | ('[' | '(') NumberLiteral ',' NumberLiteral? (']' | ')')
  //  | ('[' | '(') NumberLiteral? ',' NumberLiteral (']' | ')')
  //  ;
  //

  public Tree parseModuleVersion() {
    if(!poll(kColon)) {
      return expected(kColon);
    }

    boolean isBracket;

    if(poll(kLBrac)) {
      isBracket = true;
    } else if(poll(kLParen)) {
      isBracket = false;
    } else {
      final Tree number = parseNumberLiteral();

      //TODO(joa): NumberLiteral

      return error();
    }

    if(poll(kComma)) {
      // ?,
      final Tree x = parseNumberLiteral();

      if(poll(kRBrac)) {
        //?,x]
        return error();//"TODO ?,x]");
      } else if(poll(kRParen)) {
        //?,x)
        return error();//"TODO ?,x)");
      } else {
        return syntaxError(InvalidSyntaxReport.kExpectRBracRParen);
      }
    } else {
      // ?x
      final Tree x = parseNumberLiteral();

      if(poll(kComma)) {
        //[x,
        if(poll(kRBrac)) {
          // [x,]
          return error();//"TODO ?x,]");
        } else if(poll(kRParen)) {
          // [x,)
          return error();//"TODO ?x,)");
        } else {
          //[x,y
          final Tree y = parseNumberLiteral();

          if(poll(kRBrac)) {
            //[x,y]
            return error();//"TODO ?x,y]");
          } else if(poll(kRParen)) {
            //[x,y)
            return error();//"TODO ?x,y)");
          } else {
            return syntaxError(InvalidSyntaxReport.kExpectRBracRParen);
          }
        }
      } else if(isBracket && poll(kRBrac)) {
        //[x]
        //return
        return error();//"TODO [x]");
      } else {
        return syntaxError(InvalidSyntaxReport.kExpectCommaRBrac);
      }
    }
  }

  //
  // Block
  //  : '{' NEWLINE (Block NEWLINE)+ '}'
  //  | Expression
  //  | Declaration
  //  ;
  //
  public Tree parseBlock() {
    if(poll(kLBrace)) {
      if(!poll(kNewLine)) {
        return expected(kNewLine);
      }

      final Block block = init(new Block());
      final List<Tree> expressions = block.expressions;

      do {
        expressions.add(parseBlock());
        if(!poll(kNewLine)) {
          return expected(kNewLine);
        }
      } while(!peek(kRBrace));

      if(!poll(kRBrace)) {
        return expected(kRBrace);
      }

      return block;
    } else {
      final Tree declaration = parseDeclarationOpt();

      if(declaration == null) {
        return parseExpression();
      }

      return declaration;
    }
  }

  //
  // Declaration
  //  : FunctionDeclaration
  //  | Class
  //  | Variable
  //  ;
  //
  public Tree parseDeclarationOpt() {
    pollAllNewLine();

    int flags = 0;

    if(poll(kOverride)) {
      flags |= Tree.ACC_OVERRIDE;
    }

    if(peek(kDef)) {
      return parseFunction(Tree.ACC_PUBLIC);
    } else if(peek(kClass)) {
      return parseClass(Tree.ACC_PUBLIC);
    } else if(peek(kVal) || peek(kVar)) {
      return parseVariable(Tree.ACC_PUBLIC);
    } else if(peekVisibility()) {
      if(poll(kPublic)) {
        flags |= Tree.ACC_PUBLIC;
      } else if(poll(kPrivate)) {
        flags |= Tree.ACC_PRIVATE;
      } else if(poll(kProtected)) {
        flags |= Tree.ACC_PROTECTED;
      } else if(poll(kInternal)) {
        flags |= Tree.ACC_INTERNAL;
      }

      flags |= parseDeclarationFlags();

      if(peek(kDef)) {
        return parseFunction(flags);
      } else if(peek(kClass)) {
        return parseClass(flags);
      } else if(peek(kVal) || peek(kVar)) {
        return parseVariable(flags);
      } else {
        return syntaxError(InvalidSyntaxReport.kExpectDeclaration);
      }
    } else if(peekDeclarationFlag()) {
      flags |= Tree.ACC_PUBLIC;
      flags |= parseDeclarationFlags();

      if(peek(kDef)) {
        return parseFunction(flags);
      } else if(peek(kClass)) {
        return parseClass(flags);
      } else if(peek(kVal) || peek(kVar)) {
        return parseVariable(flags);
      } else {
        return syntaxError(InvalidSyntaxReport.kExpectDeclaration);
      }
    } else if(peek(kModule)) {
      return parseModule();
    } else {
      return null;
    }
  }

  public int parseDeclarationFlags() {
    int flags = 0;

    while(peekDeclarationFlag()) {
      if(poll(kVirtual)) {
        flags |= Tree.ACC_VIRTUAL;
      }

      if(poll(kImmutable)) {
        flags |= Tree.ACC_IMMUTABLE;
      }

      if(poll(kIdempotent)) {
        flags |= Tree.ACC_IDEMPOTENT;
      }

      if(poll(kSealed)) {
        flags |= Tree.ACC_SEALED;
      }

      if(poll(kPure)) {
        flags |= Tree.ACC_PURE;
      }

      if(poll(kPartial)) {
        flags |= Tree.ACC_PARTIAL;
      }

      if(poll(kNative)) {
        flags |= Tree.ACC_NATIVE;
      }
    }

    return flags;
  }

  // this is inconsistent since '{' Block '}' is not true. '{' and '}' are
  // already part of the block but they must be present!
  //
  // Definition
  //  : 'def' Identifier TypeParameterList? '(' ParameterList ')' Type '=' Block
  //  | 'def' Identifier TypeParameterList? '(' ParameterList ')' '{' Block '}'
  //
  public Tree parseFunction(int flags) {
    if(!poll(kDef)) {
      return expected(kDef);
    }

    final Name name = parseName();
    final List<Tree> typeParameters = new ArrayList<>();

    if(peek(kLBrac)) {
      final Optional<Tree> error = parseTypeParameterList(typeParameters);

      if(error.isPresent()) {
        return error.get();
      }
    }

    final List<Tree> parameters = new ArrayList<>();

    if(poll(kLParen)) {
      if(!peek(kRParen)) {
        parseParameterList(parameters);
      }

      if(!poll(kRParen)) {
        return expected(kRParen);
      }
    } else {
      flags |= Tree.ACC_IDEMPOTENT;
    }

    Tree block = null;
    Optional<Tree> type = Optional.empty();

    if(poll(kColon)) {
      type = Optional.of(parseType());

      if(poll(kAssign)) {
        block = parseBlock();
      } else if(peek(kNewLine)) {
        if((flags & Tree.ACC_NATIVE) == 0) {
          flags |= Tree.ACC_ABSTRACT;
        }
      } else {
        return syntaxError(InvalidSyntaxReport.kExpectAssignOrNewLine);
      }
    } else if(peek(kLBrace)) {
      block = parseBlock();
    } else if(peek(kNewLine)) {
      if((flags & Tree.ACC_NATIVE) == 0) {
        flags |= Tree.ACC_ABSTRACT;
      }
    } else if(poll(kAssign)) {
      //must override a base!
      block = parseBlock();
    }

    // must override a base method!

    return init(new FunctionDeclaration(
        name,
        Optional.ofNullable(block),
        type,
        parameters,
        typeParameters
    )).withFlags(flags);
  }

  //
  // Visibility
  //  : Public
  //  | Private
  //  | Protected
  //  | Internal
  //  ;
  //
  // Class
  //  : Visibility? 'virtual'? 'native'? 'class' Identifier TypeParameter?
  //      Constructor Extends? '{' NEWLINE (Definition NEWLINE)* '}'
  //
  public Tree parseClass(final int flags) {
    if(0 == flags) {
      return internalError();
    }

    if(!poll(kClass)) {
      return expected(kClass);
    }

    final Name name = parseName();
    final Class klass = init(new Class(name));

    if(peek(kLBrac)) {
      final Optional<Tree> error = parseTypeParameterList(klass.typeParameters);

      if(error.isPresent()) {
        return error.get();
      }
    }

    if(poll(kLParen)) {
      //TODO(joa): implement constructor
      if(!poll(kRParen)) {
        return expected(kRParen);
      }
    }


    if(poll(kColon)) {
      do {
        klass.bases.add(parseType());
      } while(poll(kComma));
    }

    if(poll(kLBrace)) {
      if(!poll(kNewLine)) {
        return expected(kNewLine);
      }

      final List<Tree> members = klass.members;

      while(!poll(kRBrace)) {
        pollAllNewLine();
        final Tree declaration = parseDeclarationOpt();
        if(null != declaration) {
          members.add(declaration);
        }
        if(!poll(kNewLine)) {
          return expected(kNewLine);
        }
      }
    }

    return klass.withFlags(flags);
  }

  public Tree parseVariable(final int flags) {
    boolean modifiable;
    boolean forced = false;

    if(poll(kVar)) {
      modifiable = true;
    } else if(poll(kVal)) {
      modifiable = false;
    } else {
      return syntaxError(InvalidSyntaxReport.kExpectVariable);
    }

    final Name name = parseName();
    Optional<Tree> type = Optional.empty();

    if(poll(kColon)) {
      // Optional type is present.
      type = Optional.of(parseType());
    }

    Optional<Tree> init = Optional.empty();

    if(poll(kAssign)) {
      forced = poll(kForce);
      init = Optional.of(parseExpression());
    }

    return init(new Variable(name, type, init))
        .withFlags(flags
            | (modifiable ? 0 : Tree.ACC_IMMUTABLE)
            | (forced ? Tree.ACC_FORCE : 0));
  }

  // Parameter
  //  : Identifier (':' Type)? ('=' Expression)?
  public Tree parseParameter() {
    final Name name = parseName();

    final Optional<Tree> type;

    if(poll(kColon)) {
      type = Optional.of(parseType());
    } else {
      type = Optional.empty();
    }

    final Optional<Tree> init;

    if(poll(kAssign)) {
      init = Optional.of(parseExpression());
    } else {
      init = Optional.empty();
    }

    return init(new Parameter(name, type, init));
  }

  //
  // A special case for the expression grammar: currently I am lazy and
  // the fact that infix calls are left-associative is only expressed
  // via a boolean that makes the production less greedy. So
  // the option Expression Call* is always valid but it accepts only calls
  // in the form of "foo.bar(baz)" if allowInfixCall is false. Otherwise
  // it accepts "foo.bar(baz)" and "foo bar baz".
  //
  // Expression
  //  : NEWLINE* '(' Expression ')'
  //  | NEWLINE* Tuple
  //  | NEWLINE* IfExpression
  //  | NEWLINE* Expression Select*
  //  | NEWLINE* Expression Call*
  //  | NEWLINE* PrimaryExpression
  //
  public Tree parseExpression() {
    return parseExpression(true);
  }

  public Tree parseExpression(final boolean allowInfixCall) {
    pollAllNewLine();

    Tree expression = null;

    if(poll(kLParen)) {
      expression = parseExpression();

      if(peek(kComma)) {
        do {
          if(!poll(kComma)) {
            return expected(kComma);
          }

          parseExpression();
        } while(!poll(kRParen));
      } else {
        if(!poll(kRParen)) {
          return expected(kRParen);
        }
      }
    } else if(peek(kLBrace)) {
      expression = parseBlock();
    } else if(peek(kIf)) {
      expression = parseIfExpression();
    } else if(peekPrimaryExpression()) {
      expression = parsePrimaryExpression();
    } else if(peek(kNew)) {
      expression = parseNew();
    }

    return continueWithExpression(expression, allowInfixCall);
  }

  //
  // PrimaryExpression
  //  : Identifier
  //  | NumberLiteral
  //  | BooleanLiteral
  //  | StringLiteral
  //  | This
  //
  public Tree parsePrimaryExpression() {
    if(peek(kIdentifier)) {
      return parseIdentifier();
    } else if(peek(kNumberLiteral)) {
      return parseNumberLiteral();
    } else if(peek(kTrue) || peek(kYes) ||
        peek(kFalse) || peek(kNo)) {
      return parseBooleanLiteral();
    } else if(peek(kStringLiteral)) {
      return parseStringLiteral();
    } else if(peek(kThis)) {
      return parseThis();
    } else {
      return syntaxError(InvalidSyntaxReport.kExpectPrimaryExpression);
    }
  }

  public Tree continueWithExpression(Tree expression, final boolean allowInfixCall) {
    if(expression == null) {
      return syntaxError(InvalidSyntaxReport.kExpectExpression);
    } else {
      while(true) {
        if(peek(kDot)) {
          expression =  parseSelect(expression);
        } if(peek(kHash)) {
          //TODO(joa): implement ref
          //auto ref = parseRef(expression);
          //expression = ref;
          expression = error();
        } else if(peek(kLParen) ||
            (allowInfixCall && peek(kIdentifier))) {
          expression = parseCall(expression);
        } else if(peek(kAssign)) {
          expression = parseAssign(expression);
        } else {
          break;
        }
      }

      return expression;
    }
  }

  //
  // Select
  //  : Expression '.' Identifier
  //
  public Tree parseSelect(final Tree expression) {
    if(!poll(kDot)) {
      return expected(kDot);
    }

    return init(new Select(expression, parseName()));
  }

  //
  // Assign
  //  : Expression '=' 'force'? Expression
  //
  //
  public Tree parseAssign(final Tree target) {
    if(!poll(kAssign)) {
      return expected(kAssign);
    }

    final int flags = poll(kForce) ? Tree.ACC_FORCE : 0;

    return init(new Assign(target, parseExpression())).withFlags(flags);
  }

  //
  // Call
  //  : Expression '(' ArgumentList ')'
  //  | Expression Identifier SingleArgument
  //
  public Tree parseCall(final Tree callee) {
    if(poll(kLParen)) {
      final Call result = init(new Call(callee));

      if(!peek(kRParen)) {
        parseArgumentList(result.arguments);
      }

      if(!poll(kRParen)) {
        return expected(kRParen);
      }

      return result;
    } else if(peek(kIdentifier)) {
      final Name name = parseName();
      final Select select = init(new Select(callee, name));
      final Call result = init(new Call(select));
      result.arguments.add(parseSingleArgument());
      return result;
    } else {
      return expected(kLParen);
    }
  }

  //
  // Argument
  //  : Expression
  //  | Identifier '=' Expression
  //  | AnonymousFunctionExpression
  //  | Identifier '=' AnonymousFunctionExpression
  //
  public Tree parseArgument() {
    if(peek(kIdentifier)) {
      final Optional<Name> name = parseNameOpt();

      if(poll(kAssign)) {
        if(peek(kLBrace)) {
          return init(new Argument(parseAnonymousFunctionExpression(), name));
        } else {
          return init(new Argument(parseExpression(), name));
        }
      } else {
        return init(new Argument(
            continueWithExpression(init(new Identifier(name.orElse(names.predef_empty))), true), Optional.empty()));
      }
    } else {
      if(peek(kLBrace)) {
        return init(new Argument(parseAnonymousFunctionExpression(), Optional.empty()));
      } else {
        return init(new Argument(parseExpression(), Optional.empty()));
      }
    }
  }

  //
  // SingleArgument
  //  : Expression
  //  | AnonymousFunctionExpression
  //
  public Tree parseSingleArgument() {
    if(peek(kLBrace)) {
      return init(new Argument(parseAnonymousFunctionExpression(), Optional.empty()));
    } else {
      return init(new Argument(parseExpression(false), Optional.empty()));
    }
  }

  //
  // AnonymousFunctionExpression
  //  : '{' NEWLINE* AnonymousFunctionParameterList
  //    (':' Type?) ->' Block NEWLINE* '}'
  //
  public Tree parseAnonymousFunctionExpression() {
    if(!poll(kLBrace)) {
      return expected(kLBrace);
    }

    pollAllNewLine();

    final List<Tree> parameters = new ArrayList<>();

    parseAnonymousFunctionParameterList(parameters);
    Optional<Tree> returnType = Optional.empty();
    if(poll(kColon)) {
      returnType = Optional.of(parseType());
    }
    if(!poll(kRArrow)) {
      return expected(kRArrow);
    }
    final Tree block = parseBlock();
    pollAllNewLine();
    if(!poll(kRBrace)) {
      return expected(kRBrace);
    }
    return init(new FunctionExpression(block, returnType, parameters, new ArrayList<>(0))).
        withFlags(Tree.ACC_PUBLIC);
  }

  //
  // AnonymousFunctionParameter
  //  : Identifier (':' Type)? ('=' Expression)?
  //
  public Tree parseAnonymousFunctionParameter() {
    return parseParameter();
  }

  //
  // This
  //  : 'this'
  //
  public Tree parseThis() {
    if(poll(kThis)) {
      return init(new This());
    }

    return expected(kThis);
  }

  //
  // BooleanLiteral
  //  : 'yes'
  //  | 'no'
  //  | 'true'
  //  | 'false'
  //
  public Tree parseBooleanLiteral() {
    if(poll(kTrue) || poll(kYes)) {
      return init(new LiteralTrue());
    } else if(poll(kFalse) || poll(kNo)) {
      return init(new LiteralFalse());
    } else {
      return syntaxError(InvalidSyntaxReport.kExpectBooleanLiteral);
    }
  }

  public Tree parseNumberLiteral() {
    //TODO(joa): fix this
    if(!peek(kNumberLiteral)) {
      return expected(kNumberLiteral);
    }

    final String value = new String(scanner.value(), 0, scanner.lengthOfValue());
    final Tree result = init(new LiteralReal(LiteralNumeric.kFloat64, Double.parseDouble(value)));
    advance();
    return result;
  }

  public Tree parseStringLiteral() {
    if(!peek(kStringLiteral)) {
      return expected(kStringLiteral);
    }

    final Tree result = init(new LiteralString(new String(scanner.value(), 0, scanner.lengthOfValue())));
    advance();
    return result;
  }

  //
  // IfExpression
  //  : if '{' NEWLINE (IfCase NEWLINE)+ '}'
  //  | if IfCase
  //
  public Tree parseIfExpression() {
    if(!poll(kIf)) {
      return expected(kIf);
    }

    final If result = init(new If());

    if(poll(kLBrace)) {
      if(!poll(kNewLine)) {
        return expected(kNewLine);
      }

      final List<Tree> cases = result.cases;

      do {
        cases.add(parseIfCase());
        if(!poll(kNewLine)) {
          return expected(kNewLine);
        }
      } while(!peek(kRBrace));
      if(!poll(kRBrace)) {
        return expected(kRBrace);
      }
    } else {
      result.cases.add(parseIfCase());
    }

    return result;
  }

  //
  // IfCase
  //  : Expression '->' Expression
  //
  public Tree parseIfCase() {
    final Tree condition = parseExpression();
    if(!poll(kRArrow)) {
      return expected(kRArrow);
    }
    return init(new IfCase(condition, parseExpression()));
  }

  //
  // New
  //  : 'new' Type ( '(' ArgumentList? ')' )?
  public Tree parseNew() {
    if(!poll(kNew)) {
      return expected(kNew);
    }

    final New neu = init(new New(parseType()));

    if(poll(kLParen)) {
      if(!poll(kRParen)) {
        parseArgumentList(neu.arguments);
        if(!poll(kRParen)) {
          return expected(kRParen);
        }
      }
    }

    return neu;
  }
  //
  // Type
  //  : Identifier ('[' Type ']')?
  //  | '-> Type
  //  | Type '->' Type
  //  | '(' Type (',' Type)+ ')' '->' Type
  //  | '(' Type (',' Type)+ ')'
  //
  public Tree parseType() {
    if(poll(kLParen)) {
      List<Tree> parameters = new ArrayList<>(2);

      parameters.add(parseType());

      do {
        if(!poll(kComma)) {
          return expected(kComma);
        }
        parameters.add(parseType());
      } while(!poll(kRParen));

      if(poll(kRArrow)) {
        // (T0, T1, ..., Tn) -> R is a TypeReference of function
        final List<Tree> functionTypeParameters = new ArrayList<>(parameters.size()+1);
        functionTypeParameters.addAll(parameters);
        functionTypeParameters.add(parseType());
        return init(new TypeReference(names.predef_function(parameters.size()), functionTypeParameters));
      }

      return init(new TypeReference(names.predef_tuple(parameters.size()), parameters));
    } else if(poll(kRArrow)) {
      final List<Tree> parameters = new ArrayList<>(1);
      parameters.add(parseType());
      return init(new TypeReference(names.predef_function(0), parameters));
    } else {
      int flags = poll(kImmutable) ? Tree.ACC_IMMUTABLE : 0;
      flags |= poll(kPure) ? Tree.ACC_PURE : 0;
      final Name name = parseQualifiedName();
      final List<Tree> parameters = new ArrayList<>(0);

      if(poll(kLBrac)) {
        do {
          parameters.add(parseType());
        } while(poll(kComma));
        if(!poll(kRBrac)) {
          return expected(kRBrac);
        }
      }

      final Tree lhs = init(new TypeReference(name, parameters)).withFlags(flags);

      if(poll(kRArrow)) {
        // T0, T1, ..., Tn -> R is a TypeReference of function
        final List<Tree> functionTypeParameters = new ArrayList<>(2);
        functionTypeParameters.add(lhs);
        functionTypeParameters.add(parseType());
        return init(new TypeReference(names.predef_function(1), functionTypeParameters));
      } else {
        return lhs;
      }
    }
  }

  //
  // TypeParameterList
  //  : '[' TypeParameter (',' TypeParameter)* ']'
  //
  public Optional<Tree> parseTypeParameterList(final List<Tree> list) {
    if(!poll(kLBrac)) {
      return Optional.of(expected(kLBrac));
    }

    do {
      list.add(parseTypeParameter());
    } while(poll(kComma));

    if(!poll(kRBrac)) {
      return Optional.of(expected(kRBrac));
    }

    return Optional.empty();
  }

  //
  // TypeParameter
  //  : Identifier
  //  | Identifier '->' Type
  //  | Identifier '<-' Type
  //
  public Tree parseTypeParameter() {
    int variance = TypeParameter.kInvariant;

    Name peekIdentifier = null;

    if(peek(kIdentifier)) {
      final Name name = parseName();
      if(name == names.predef_out) {
        variance = TypeParameter.kCovariant;
      } else if(name == names.predef_in) {
        variance = TypeParameter.kContravariant;
      } else {
        peekIdentifier = name;
      }
    }

    final Name name = null == peekIdentifier ? parseName() : peekIdentifier;

    if(poll(kRArrow)) {
      final Tree bound = parseType();
      return init(new TypeParameter(name, variance, Optional.of(bound), TypeParameter.kBoundExtends));
    } else if(poll(kLArrow)) {
      final Tree bound = parseType();
      return init(new TypeParameter(name, variance, Optional.of(bound), TypeParameter.kBoundBase));
    } else {
      return init(new TypeParameter(name, variance, Optional.empty(), TypeParameter.kBoundNone));
    }
  }

  public Tree parseIdentifier() {
    if(!peek(kIdentifier)) {
      return expected(kIdentifier);
    }

    return init(new Identifier(parseQualifiedName()));
  }

  private Name parseQualifiedName() {
    if(!peek(kIdentifier)) {
      return names.predef_empty;
    }

    final StringBuilder qualifiedName = new StringBuilder();

    do {
      qualifiedName.append(scanner.value(), 0, scanner.lengthOfValue());
      advance();
      if(poll(kDot)) {
        qualifiedName.append('.');
      } else {
        break;
      }
    } while(peek(kIdentifier));

    final int n = qualifiedName.length();
    final char[] chars = new char[n];
    qualifiedName.getChars(0, n, chars, 0);

    return names.get(chars, 0, n, /*copy=*/false);
  }

  private Optional<Name> parseNameOpt() {
    if(!peek(kIdentifier)) {
      return Optional.empty();
    }

    return Optional.of(parseName());
  }

  private Name parseName() {
    if(!peek(kIdentifier)) {
      return names.predef_empty;
    }

    final Name name = names.get(scanner.value(), 0, scanner.lengthOfValue(), /*copy=*/true);
    advance();

    return name;
  }

  private void advance() {
    do {
      currentToken = scanner.nextToken();
    } while(isIgnored(currentToken));
  }

  private boolean isIgnored(final int token) {
    return token == kWhitespace || token == kCommentSingle || token == kCommentMulti;
  }

  private boolean peek(final int token) {
    return currentToken == token;
  }
  private boolean poll(final int token) {
    if(peek(token)) {
      advance();
      return true;
    }

    return false;
  }

  private void pollAllNewLine() {
    //noinspection StatementWithEmptyBody
    while(poll(kNewLine));
  }

  private Tree error() {
    return init(new Error());
  }

  private Tree expected(final int token) {
    final Tree errorTree = error();
    reporter.invalidSyntax(token, currentToken, errorTree.position);
    return errorTree;
  }

  private Tree syntaxError(final int kind) {
    final Tree tree = error();
    reporter.invalidSyntax(kind, -1, currentToken, tree.position);
    return tree;
  }

  private Tree internalError() {
    final Tree errorTree = error();
    reporter.invalidSyntax(InvalidSyntaxReport.kInternalError, -1, -1, errorTree.position);
    return errorTree;
  }

  private <T extends Tree> T init(final T tree) {
    tree.position =
        new Position(unit, scanner.posLine(), scanner.posColumn());
    return tree;
  }
}
