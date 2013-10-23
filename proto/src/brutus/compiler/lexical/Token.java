package brutus.compiler.lexical;

/**
 *
 */
public final class Token {
  public static final int kEof = 0;
  public static final int kError = 1;
  public static final int kNewLine = 2;
  public static final int kWhitespace = 3;

  public static final int kIdentifier = 4;
  public static final int kNumberLiteral = 5;
  public static final int kStringLiteral = 6;

  public static final int kCommentSingle = 7;
  public static final int kCommentMulti = 8;

  // Control characters:
  public static final int kSemicolon = 9;  // ;
  public static final int kComma = 10;         // ,
  public static final int kLParen = 11;            // (
  public static final int kRParen = 12;           // )
  public static final int kLBrac = 13;            // [
  public static final int kRBrac = 14;             // ]
  public static final int kLBrace = 15;            // {
  public static final int kRBrace = 16;           // }
  public static final int kDot = 17;              // .
  public static final int kColon = 18;               // :
  public static final int kAssign = 19;            // =
  public static final int kLArrow = 20;           // <- and ?
  public static final int kRArrow = 21;           // -> and ?
  public static final int kHash = 22;             // #

  // Keywords:
  public static final int kThis = 23;
  public static final int kVal = 24;
  public static final int kVar = 25;
  public static final int kDef = 26;
  public static final int kFor = 27;
  public static final int kWhile = 28;
  public static final int kYield = 29;
  public static final int kTrue = 30;
  public static final int kFalse = 31;
  public static final int kYes = 32;
  public static final int kNo = 33;
  public static final int kIf = 34;
  public static final int kNew = 35;
  public static final int kOn = 36;
  public static final int kClass = 37;
  public static final int kTrait = 38;
  public static final int kVirtual = 39;
  public static final int kPublic = 40;
  public static final int kPrivate = 41;
  public static final int kProtected = 42;
  public static final int kInternal = 43;
  public static final int kNative = 44;
  public static final int kForce = 45;
  public static final int kModule = 46;
  public static final int kRequire = 47;
  public static final int kPure = 48;
  public static final int kImmutable = 49;
  public static final int kOverride = 50;
  public static final int kIn = 51;
  public static final int kOut = 52;
  public static final int kSealed = 53;
  public static final int kIdempotent = 54;
  public static final int kPartial = 55;

  public static boolean hasValue(final int token) {
    switch(token) {
      case kIdentifier:
      case kNumberLiteral:
      case kStringLiteral:
      case kCommentMulti:
      case kCommentSingle:
        return true;
      default:
        return false;
    }
  }

  public static String toString(final int token) {
    switch(token) {
      case kEof: return "Eof";
      case kError: return "Error";
      case kNewLine: return "NewLine";
      case kWhitespace: return "Whitespace";
      case kIdentifier: return "Identifier";
      case kNumberLiteral: return "NumberLiteral";
      case kStringLiteral: return "StringLiteral";
      case kCommentSingle: return "CommentSingle";
      case kCommentMulti: return "CommentMulti";
      case kSemicolon: return "Semicolon";
      case kComma: return "Comma";
      case kLParen: return "LParen";
      case kRParen: return "RParen";
      case kLBrac: return "LBrac";
      case kRBrac: return "RBrac";
      case kLBrace: return "LBrace";
      case kRBrace: return "RBrace";
      case kDot: return "Dot";
      case kColon: return "Colon";
      case kAssign: return "Assign";
      case kLArrow: return "LArrow";
      case kRArrow: return "RArrow";
      case kHash: return "Hash";
      case kThis: return "This";
      case kVal: return "Val";
      case kVar: return "Var";
      case kDef: return "Def";
      case kFor: return "For";
      case kWhile: return "While";
      case kYield: return "Yield";
      case kTrue: return "True";
      case kFalse: return "False";
      case kYes: return "Yes";
      case kNo: return "No";
      case kIf: return "If";
      case kNew: return "New";
      case kOn: return "On";
      case kClass: return "Class";
      case kTrait: return "Trait";
      case kVirtual: return "Virtual";
      case kOverride: return "Override";
      case kPublic: return "Public";
      case kPrivate: return "Private";
      case kProtected: return "Protected";
      case kInternal: return "Internal";
      case kNative: return "Native";
      case kForce: return "Force";
      case kModule: return "Module";
      case kRequire: return "Require";
      case kPure: return "Pure";
      case kImmutable: return "Immutable";
      case kIn: return "In";
      case kOut: return "Out";
      case kSealed: return "Sealed";
      case kIdempotent: return "Idempotent";
      case kPartial: return "Partial";
      default: return "<<tok:"+token+">>";
    }
  }

  private Token() {}
}
