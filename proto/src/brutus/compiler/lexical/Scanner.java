package brutus.compiler.lexical;

import brutus.compiler.util.CharStream;

import static brutus.compiler.lexical.Token.*;

/**
 *
 */
public final class Scanner {
  private static final int kBufferSize = 0x1000;

  private final CharStream chars;
  private final char[] buffer;
  private int line;
  private int column;
  private char currentChar;
  private boolean advanceWithLastChar;
  private int bufferIndex;

  public Scanner(final CharStream chars) {
    this.chars = chars;
    this.buffer = new char[kBufferSize];
  }

  public int nextToken() {
    if(canAdvance()) {
      final char currentChar = advance();

      if(isWhitespace(currentChar)) {
        // The current character represents whitespace. We consume
        // it until there is a character that is not considered
        // whitespace.

        return resulting(Scanner::isWhitespace, CharPredicate.TRUE, kWhitespace);
      } else if(isNewLine(currentChar)) {
        // When a new line character is encountered we will simply
        // update the position information and consume until there
        // is a character that is not considered a new line.
        //
        // This also means that '\n' is considered equal to "\n\n"
        // and allows people to use more new lines if they want to.
        //
        // It is important to note the side effect which resets the
        // position information.
        ++line;
        column = 0;

        return resulting(
            Scanner::isNewLine,
            (final char c) -> {
              ++line;
              column = 0;
              return true;
            },
            kNewLine);
      } else if(isNumberStart(currentChar)) {
        return continueWithNumberStart(currentChar);
      } else if(currentChar == '"') {
        return continueWithString();
      } else if(isIdentifierStart(currentChar)) {
        return continueWithIdentifierStart(currentChar, /*isOperator=*/false);
      } else if(currentChar == '=') {
        if(isOperator(advance())) {
          rewind();
          return continueWithIdentifierStart(currentChar, /*isOperator=*/true);
        } else {
          rewind();
          return kAssign;
        }
      } else if(currentChar == '-') {
        if(advance() == '>') {
          // One could decide to allow operators like ->> but then again this
          // becomes really confusing when looking at code like "x -> x ->> y" so
          // the call is to disallow other operators that start with ->
          return kRArrow;
        } else {
          rewind();
          return continueWithIdentifierStart(currentChar, /*isOperator=*/false);
        }
      } else if('/' == currentChar) {
        return continueWithSlash(currentChar);
      } else if('`' == currentChar) {
        // Parse until `. No escaping allowed. The resulting identifier
        // will contain the name sans ` characters.
        return continueWithBackTick();
      } else if(isOperator(currentChar)) {
        // The generic operator handling comes after '=', '-' and '/'
        return continueWithIdentifierStart(currentChar, /*isOperator=*/true);
      } else {
        @SuppressWarnings("UnnecessaryLocalVariable")
        final int charCode = currentChar;

        if(charCode >= 0x00 && charCode<= 0x80) {
          return kSingleCharacterTokens[charCode];
        } else {
          return kError;
        }
      }
    }

    return kEof;
  }

  public char[] value() {
    return buffer;
  }

  public int lengthOfValue() {
    return bufferIndex;
  }

  public int posLine() {
    return line + 1;
  }

  public int posColumn() {
    return column + 1;
  }

  private void resetBuffer() {
    bufferIndex = 0;
  }

  private void beginBuffer(final char c) {
    bufferIndex = 1;
    buffer[0] = c;
  }

  private boolean continueBuffer(final char c) {
    if(bufferIndex >= buffer.length) {
      return false;
    }

    buffer[bufferIndex++] = c;
    return true;
  }

  private boolean canAdvance() {
    return advanceWithLastChar || chars.hasNext();
  }

  private char advance() {
    if(!canAdvance()) {
      return '\0';
    }

    if(advanceWithLastChar) {
      advanceWithLastChar = false;
    } else {
      currentChar = chars.next();
    }

    ++column;
    return currentChar;
  }

  private void rewind() {
    if(advanceWithLastChar) {
      throw new IllegalStateException();
    }

    advanceWithLastChar = true;
    --column;
  }

  private int continueWithNumberStart(final char currentChar) {
    beginBuffer(currentChar);

    // We allow only 0xffffff not 0XFFFFFF, not 0xFFFFFF
    final char secondChar = advance();

    boolean integer = true;

    if(secondChar == 'x') {
      // Parse hexadecimal literal

      if(!continueBuffer(secondChar)) {
        return kError;
      }

      while(canAdvance()) {
        final char hexChar = advance();

        if(isDigit(hexChar) || (hexChar >= 'a' && hexChar <= 'f')) {
          if(!continueBuffer(hexChar)) {
            return kError;
          }
        } else {
          rewind();
          break;
        }
      }
    } else if(isDigit(secondChar) || secondChar == 'e' || secondChar == '.') {
      if(!continueBuffer(secondChar)) {
        return kError;
      }

      boolean eAllowed = secondChar != 'e';
      boolean dotAllowed = eAllowed && secondChar != '.';

      if(!dotAllowed || !eAllowed) {
        integer = false;
      }

      // Parse a digit of one of the following forms:
      //
      // 12 ...
      // 1e ...
      // 1. ...

      while(canAdvance()) {
        final char nextChar = advance();

        if(isDigit(nextChar)) {
          if(!continueBuffer(nextChar)) {
            return kError;
          }
        } else if(eAllowed && nextChar == 'e') {
          eAllowed = false;
          dotAllowed = false;
          integer = false;

          if(!continueBuffer(nextChar)) {
            return kError;
          }

          final char nextCharAfterE = advance();

          if(nextCharAfterE == '-') {
            // Special treatment for the '-' character:
            // It is allowed immediately after the 'e'
            // character but nowhere else.
            //
            // If we find a '-' character we need at least
            // another digit or the literal is illegal.

            if(!continueBuffer(nextCharAfterE)) {
              return kError;
            }

            final char nextCharAfterMinus = advance();

            if(isDigit(nextCharAfterMinus)) {
              if(!continueBuffer(nextCharAfterMinus)) {
                return kError;
              }
            } else {
              // This is an illegal number literal since we
              // have something of the form "1e-" instead
              // of "1e-1".
              rewind();
              return kError;
            }
          } else if(isDigit(nextCharAfterE)) {
            if(!continueBuffer(nextCharAfterE)) {
              return kError;
            }
          } else {
            // We have a literal of the form "1e" instead of "1e1".
            rewind();
            return kError;
          }
        } else if(dotAllowed && nextChar == '.') {
          dotAllowed = false;
          integer = false;
          if(!continueBuffer(nextChar)) {
            return kError;
          }
        } else {
          rewind();
          break;
        }
      }
    } else {
      // This was only a single digit.
      rewind();
    }

    // Check for type information
    if(canAdvance()) {
      final char suffixChar = advance();

      switch(suffixChar) {
        case 'u': {
          if(canAdvance()) {
            final char secondSuffixChar = advance();
            switch (secondSuffixChar) {
              case 'b':
                return kNumberLiteral; //uint8
              case 's':
                return kNumberLiteral; //uint16
              case 'l':
                return kNumberLiteral; //uint64
            }
          }
          return kNumberLiteral; //uint32
        }
        case 'f':
          return kNumberLiteral; //float
        case 'd':
          return kNumberLiteral; //double
        case 'b':
          return kNumberLiteral; //int8
        case 's':
          return kNumberLiteral; //int16
        case 'l':
          return kNumberLiteral; //int64
        default:
          rewind();
      }
    }

    if(integer) {
      return kNumberLiteral; //int32
    } else {
      return kNumberLiteral; //double
    }
  }

  private static final int kIdentifierOrOperator = 0;
  private static final int kOperatorOnly = 1;
  private static final int kIdentifierOnly = 2;

  private int continueWithIdentifierStart(final char currentChar, final boolean isOperator) {
    beginBuffer(currentChar);

    int state;
    boolean containsOperator = isOperator;

    if(isOperator) {
      state = kOperatorOnly;
    } else if(currentChar == '_') {
      state = kIdentifierOrOperator;
    } else {
      state = kIdentifierOnly;
    }

    while(canAdvance()) {
      final char nextChar = advance();

      if(state == kIdentifierOrOperator) {
        if(isIdentifierPart(nextChar)) {
          state = nextChar == '_' ? kIdentifierOrOperator : kIdentifierOnly;
        } else if(isOperator(nextChar)) {
          state = kOperatorOnly;
          containsOperator = true;
        } else {
          rewind();
          break;
        }
      } else if(state == kOperatorOnly) {
        if(!isOperator(nextChar)) {
          rewind();
          break;
        }
      } else { // state == kIdentifierOnly
        if(isIdentifierPart(nextChar)) {
          state = nextChar == '_' ? kIdentifierOrOperator : kIdentifierOnly;
        } else {
          rewind();
          break;
        }
      }

      if(!continueBuffer(nextChar)) {
        return kError;
      }
    }

    //TODO(joa): optimize keyword lookup
    if(!containsOperator) {
      // No keyword contains an operator so we can skip the
      // check if the identifier contains an operator.
      for(int i = 0; i < kKeywordChars.length; ++i) {
        if(equalsContentInBuffer(kKeywordChars[i])) {
          return kKeywordTokens[i];
        }
      }
    }

    return kIdentifier;
  }

  private int continueWithSlash(final char currentChar) {
    beginBuffer(currentChar);

    final char nextChar = advance();

    if(nextChar == '/') {
      if(!continueBuffer(nextChar)) {
        return kError;
      }

      while(canAdvance()) {
        char singleLineChar = advance();

        if(isNewLine(singleLineChar)) {
          break;
        }

        if(!continueBuffer(singleLineChar)) {
          return kError;
        }
      }

      return kCommentSingle;
    } else if(nextChar == '*') {
      if(!continueBuffer(nextChar)) {
        return kError;
      }

      int openComments = 1;

      while(openComments > 0 && canAdvance()) {
        final char commentChar = advance();

        if(!continueBuffer(commentChar)) {
          return kError;
        }

        if(commentChar == '/') {
          if(advance() == '*') {
            if(!continueBuffer('*')) {
              return kError;
            }
            ++openComments;
          } else {
            rewind();
          }
        } else if(commentChar == '*') {
          if(advance() == '/') {
            if(!continueBuffer('/')) {
              return kError;
            }
            --openComments;
          } else {
            rewind();
          }
        }
      }

      if(openComments > 0) {
        // Reached premature EOF.
        // We can ignore this if we do not want to be
        // too pedantic.

        return kError;
      }

      return kCommentMulti;
    } else {
      rewind();
      return continueWithIdentifierStart(currentChar, /*isOperator=*/true);
    }
  }

  private int continueWithBackTick() {
    final char currentChar = advance();

    if(currentChar == '`') {
      // No empty name is allowed.
      return kError;
    }

    beginBuffer(currentChar);

    while(canAdvance()) {
      final char nextChar = advance();

      if(nextChar == '`') {
        // Reached the closing backtick so we have a valid
        // identifier that is not empty.
        return kIdentifier;
      } else {
        // Found an arbitrary character we need to add to the buffer.
        if(!continueBuffer(nextChar)) {
          return kError;
        }
      }
    }

    // We reach this code only in case of a premature EOF.
    return kError;
  }

  private int continueWithString() {
    // We already have one " character.

    if(!canAdvance()) {
      // If we are at the EOF we have an open string
      // literal lotering around.
      return kError;
    }

    final char currentChar = advance();

    if('"' == currentChar) {
      // We have two " characters.

      if(!canAdvance()) {
        // If we reach the EOF at this point we have an empty string.
        return kStringLiteral;
      }

      final char possibleQuote = advance();

      if('"' == possibleQuote) {
        // We have three " characters. Go for a verbatim string literal.
        //TODO(joa): implement verbatim string literal
        return kError;
      } else {
        // Empty string literal.
        rewind();
        resetBuffer();
        return kStringLiteral;
      }
    } else {
      beginBuffer(currentChar);

      boolean isEscaped = false;

      while(canAdvance()) {
        final char nextChar = advance();

        if('"' == nextChar) {
          if(isEscaped) {
            if(!continueBuffer(nextChar)) {
              return kError;
            }
            isEscaped = false;
          } else {
            return kStringLiteral;
          }
        } else if('\\' == nextChar) {
          if(isEscaped) {
            if(!continueBuffer(nextChar)) {
              return kError;
            }
            isEscaped = false;
          } else {
            isEscaped = true;
          }
        } else {
          if(isEscaped) {
            if(isDigit(nextChar)) {
              char c = nextChar;

              int i = 0;
              int code = 0;

              do {
                int value = c - '0';

                if(value > 7) {
                  // A value greater than 7 is not valid in
                  // octal notation
                  return kError;
                }

                code = (code << 3) + value;

                if(canAdvance()) {
                  c = advance();

                  if(!isDigit(c)) {
                    rewind();
                    break;
                  }
                } else {
                  // String literal is not closed, got EOF
                  return kError;
                }
              } while(++i < 3);

              if(!continueBuffer((char)code)) {
                return kError;
              }
            } else {
              switch(nextChar) {
                case '$': if(!continueBuffer('$')) { return kError; } break;
                case 'b': if(!continueBuffer('\b')) { return kError; } break;
                case 'f': if(!continueBuffer('\f')) { return kError; } break;
                case 'n': if(!continueBuffer('\n')) { return kError; } break;
                case 'r': if(!continueBuffer('\r')) { return kError; } break;
                case 't': if(!continueBuffer('\t')) { return kError; } break;
                case 'u':
                  //TODO(joa): implement uXXXX ...
                  break;
                default:
                  // Illegal escape sequence.
                  return kError;
              }
            }

            isEscaped = false;
          } else {
            if(!continueBuffer(nextChar)) {
              return kError;
            }
          }
        }
      }

      // Reached premature EOF while in a string literal.
      return kError;
    }
  }

  private int resulting(final CharPredicate condition,
                        final CharPredicate sideEffect,
                        final int result) {
    while(canAdvance()) {
      final char nextChar = advance();

      if(!condition.test(nextChar)) {
        rewind();
        break;
      } else {
        if(!sideEffect.test(nextChar)) {
          return kError;
        }
      }
    }

    return result;
  }

  private static boolean isWhitespace(final char c) {
    return c == '\u0020' || c == '\u00a0' || c == '\u2007' || c == '\u202f';
  }

  private static boolean isNewLine(final char c) {
    return c == '\n';
  }

  private static boolean isNumberStart(final char c) {
    return isDigit(c);
  }

  private static boolean isDigit(final char c) {
    return c >= '0' && c <= '9';
  }

  private static boolean isOperator(final char c) {
    return c == '!'
        || c == '$'
        || c == '%'
        || c == '&'
        || c == '*'
        || c == '+'
        || c == '-'
        || c == '/'
        || c == '<'
        || c == '>'
        || c == '?'
        || c == '\\'
        || c == '^'
        || c == '|'
        || c == '~'
        || c == '='
        || Character.getType(c) == Character.MATH_SYMBOL;
  }

  private static boolean isIdentifierStart(final char c) {
    return Character.isLetter(c) || c == '_';
  }

  private static boolean isIdentifierPart(final char c) {
    return isIdentifierStart(c) || Character.isAlphabetic(c) || Character.isDigit(c);
  }

  private boolean equalsContentInBuffer(final char[] keyword) {
    if(keyword.length != bufferIndex) {
      return false;
    }

    for(int i = 0; i < bufferIndex; ++i) {
      if(keyword[i] != buffer[i]) {
        return false;
      }
    }

    return true;
  }

  @FunctionalInterface
  private static interface CharPredicate {
    public static final CharPredicate TRUE = (char c) -> true;
    boolean test(final char c);
  }

  private static final char[][] kKeywordChars = {
      "this".toCharArray(),
      "val".toCharArray(),
      "var".toCharArray(),
      "def".toCharArray(),
      "for".toCharArray(),
      "while".toCharArray(),
      "yield".toCharArray(),
      "true".toCharArray(),
      "false".toCharArray(),
      "yes".toCharArray(),
      "no".toCharArray(),
      "<-".toCharArray(),
      "←".toCharArray(),
      "->".toCharArray(),
      "→".toCharArray(),
      "if".toCharArray(),
      "new".toCharArray(),
      "on".toCharArray(),
      "class".toCharArray(),
      "trait".toCharArray(),
      "virtual".toCharArray(),
      "public".toCharArray(),
      "private".toCharArray(),
      "protected".toCharArray(),
      "internal".toCharArray(),
      "native".toCharArray(),
      "force".toCharArray(),
      "module".toCharArray(),
      "require".toCharArray(),
      "pure".toCharArray(),
      "immutable".toCharArray(),
      "override".toCharArray(),
      "in".toCharArray(),
      "out".toCharArray(),
      "sealed".toCharArray(),
      "idempotent".toCharArray(),
      "partial".toCharArray()
  };

  private static final int[] kKeywordTokens = {
      kThis,
      kVal,
      kVar,
      kDef,
      kFor,
      kWhile,
      kYield,
      kTrue,
      kFalse,
      kYes,
      kNo,
      kLArrow,
      kLArrow,
      kRArrow,
      kRArrow,
      kIf,
      kNew,
      kOn,
      kClass,
      kTrait,
      kVirtual,
      kPublic,
      kPrivate,
      kProtected,
      kInternal,
      kNative,
      kForce,
      kModule,
      kRequire,
      kPure,
      kImmutable,
      kOverride,
      kIn,
      kOut,
      kSealed,
      kIdempotent,
      kPartial
  };

  private static final int[] kSingleCharacterTokens = {
      // 0-63
      kError, //0
      kError, //1
      kError, //2
      kError, //3
      kError, //4
      kError, //5
      kError, //6
      kError, //7
      kError, //8
      kError, //9
      kError, //10
      kError, //11
      kError, //12
      kError, //13
      kError, //14
      kError, //15
      kError, //16
      kError, //17
      kError, //18
      kError, //19
      kError, //20
      kError, //21
      kError, //22
      kError, //23
      kError, //24
      kError, //25
      kError, //26
      kError, //27
      kError, //28
      kError, //29
      kError, //30
      kError, //31
      kError, //32
      kError, //33
      kError, //34
      kHash, //35 '#'
      kError, //36
      kError, //37
      kError, //38
      kError, //39
      kLParen, //40 '('
      kRParen, //41 ')'
      kError, //42
      kError, //43
      kComma, //44 ','
      kError, //45
      kDot, //46 '.'
      kError, //47
      kError, //48
      kError, //49
      kError, //50
      kError, //51
      kError, //52
      kError, //53
      kError, //54
      kError, //55
      kError, //56
      kError, //57
      kColon, //58 ':'
      kSemicolon, //59 ';'
      kError, //60
      kError, //61
      kError, //62
      kError, //63
      // 64-127
      kError, //0
      kError, //1
      kError, //2
      kError, //3
      kError, //4
      kError, //5
      kError, //6
      kError, //7
      kError, //8
      kError, //9
      kError, //10
      kError, //11
      kError, //12
      kError, //13
      kError, //14
      kError, //15
      kError, //16
      kError, //17
      kError, //18
      kError, //19
      kError, //20
      kError, //21
      kError, //22
      kError, //23
      kError, //24
      kError, //25
      kError, //26
      kLBrac, //27 '['
      kError, //28
      kRBrac, //29 ']'
      kError, //30
      kError, //31
      kError, //32
      kError, //33
      kError, //34
      kError, //35
      kError, //36
      kError, //37
      kError, //38
      kError, //39
      kError, //40
      kError, //41
      kError, //42
      kError, //43
      kError, //44
      kError, //45
      kError, //46
      kError, //47
      kError, //48
      kError, //49
      kError, //50
      kError, //51
      kError, //52
      kError, //53
      kError, //54
      kError, //55
      kError, //56
      kError, //57
      kError, //58
      kLBrace, //59 '{'
      kError, //60
      kRBrace, //61 '}'
      kError, //62
      kError//63
  };
}
