package brutus.compiler.report;

import brutus.compiler.lexical.Token;

/**
 *
 */
public final class InvalidSyntaxReport extends Report {
  /* InvalidSyntax. Expected {tokExpected} got {tokActual} */
  public static final int kExpect = 0;

  /* InvalidSyntax. Expected 'true', 'false', 'yes' or 'no', got {tokActual} */
  public static final int kExpectBooleanLiteral = 1;

  /* InvalidSyntax. Expected expression */
  public static final int kExpectExpression = 2;

  /* Invalid Syntax. Expected primary expression */
  public static final int kExpectPrimaryExpression = 3;

  /* Invalid Syntax. Expected 'val' or 'var', got {tokActual} */
  public static final int kExpectVariable = 4;

  /* Internal parser error */
  public static final int kInternalError = 5;

  /* Invalid Syntax. Expected  '=' or '\n', got {tokActual} */
  public static final int kExpectAssignOrNewLine = 6;

  /* Invalid Syntax. Expected declaration, got {tokActual} */
  public static final int kExpectDeclaration = 7;

  public static final int kExpectCommaRBrac = 8;

  public static final int kExpectRBracRParen = 9;

  public final int kind;
  public final int tokExpected;
  public final int tokActual;

  InvalidSyntaxReport(final int kind, final int tokExpected, final int tokActual, final Position position) {
    super(position);
    this.kind = kind;
    this.tokExpected = tokExpected;
    this.tokActual = tokActual;
  }

  @Override
  public int level() {
    return ReportLevel.kError;
  }

  public String toString() {
    if(kind == kInternalError) {
      return "Internal error.";
    }

    final StringBuilder sb = new StringBuilder();

    sb.append("Invalid Syntax. Expected ");

    switch(kind) {
      case kExpect: sb.append(Token.toString(tokExpected)); break;
      case kExpectBooleanLiteral: sb.append("'true', 'false', 'yes' or 'no'"); break;
      case kExpectExpression: sb.append("expression"); break;
      case kExpectPrimaryExpression: sb.append("primary expression"); break;
      case kExpectVariable: sb.append("'var' or 'val'"); break;
      case kExpectAssignOrNewLine: sb.append("'=' or '\\n'"); break;
      case kExpectDeclaration: sb.append("declaration"); break;
      case kExpectCommaRBrac: sb.append("',' or ']'"); break;
      case kExpectRBracRParen: sb.append("']' or ')'"); break;
    }

    return sb.append(", got ").append(Token.toString(tokActual)).toString();
  }
}
