package brutus.compiler.util;

import brutus.compiler.name.Name;

import java.io.IOException;
import java.io.Writer;

import static brutus.compiler.util.Unchecked.unchecked;

/**
 *
 */
public final class CodeWriter extends Writer {
  public static final char[] kCommaSpace = ", ".toCharArray();

  private static final char[][] kIndentation = {
      {},
      "  ".toCharArray(),
      "    ".toCharArray(),
      "      ".toCharArray(),
      "        ".toCharArray(),
      "          ".toCharArray(),
      "            ".toCharArray(),
      "              ".toCharArray(),
      "                ".toCharArray(),
      "                  ".toCharArray(),
      "                    ".toCharArray(),
      "                      ".toCharArray(),
      "                        ".toCharArray(),
      "                          ".toCharArray(),
      "                            ".toCharArray(),
      "                              ".toCharArray(),
      "                                ".toCharArray(),
  };

  public static final int kMaxIndentation = kIndentation.length;

  private final Writer writer;

  private int indentLevel = 0;
  private boolean wasNewLine = false;

  public CodeWriter(final Writer writer) {
    this.writer = writer;
  }

  public void pushIndent() {
    if(++indentLevel >= kMaxIndentation) {
      indentLevel = kMaxIndentation - 1;
    }
  }

  public void popIndent() {
    if(--indentLevel < 0) {
      indentLevel = 0;
    }
  }

  public void print(final String value) {
    print(value, 0, value.length());
  }

  public void print(final String value, final int offset, final int length) {
    maybeIndent();
    unchecked(() -> writer.write(value, offset, length));
  }

  public void print(final char value) {
    maybeIndent();
    unchecked(() -> writer.write(value));
  }

  public void print(final char[] value) {
    print(value, 0, value.length);
  }

  public void print(final char[] value, final int offset, final int length) {
    maybeIndent();
    unchecked(() -> writer.write(value, offset, length));
  }

  public void print(final Name name) {
    maybeIndent();
    unchecked(() -> writer.write(name.chars, name.offset, name.count));
  }

  public void println(final String value) {
    println(value, 0, value.length());
  }

  public void println(final String value, final int offset, final int length) {
    print(value, offset, length);
    printNewLine();
  }

  public void println(final char value) {
    print(value);
    printNewLine();
  }

  public void println(final Name value) {
    print(value);
    printNewLine();
  }

  public void printNewLine() {
    print('\n');
    wasNewLine = true;
  }

  private void maybeIndent() {
    if(wasNewLine) {
      unchecked(() -> writer.write(kIndentation[indentLevel]));
      wasNewLine = false;
    }
  }

  @Override
  public void write(char[] chars, int offset, int length) throws IOException {
    writer.write(chars, offset, length);
  }

  @Override
  public void flush() throws IOException {
    writer.flush();
  }

  @Override
  public void close() throws IOException {
    writer.close();
  }

  public static interface Mixin {
    public CodeWriter writer();

    public default void pushIndent() {
      writer().pushIndent();
    }

    public default void popIndent() {
      writer().popIndent();
    }

    public default void print(final String value) {
      writer().print(value);
    }

    public default void print(final String value, final int offset, final int length) {
      writer().print(value, offset, length);
    }

    public default void print(final char value) {
      writer().print(value);
    }

    public default void print(final char[] value) {
      writer().print(value);
    }

    public default void print(final char[] value, final int offset, final int length) {
      writer().print(value, offset, length);
    }

    public default void print(final Name name) {
      writer().print(name);
    }

    public default void println(final String value) {
      writer().println(value);
    }

    public default void println(final String value, final int offset, final int length) {
      writer().println(value, offset, length);
    }

    public default void println(final char value) {
      writer().println(value);
    }

    public default void println(final Name name) {
      writer().println(name);
    }

    public default void printNewLine() {
      writer().printNewLine();
    }
  }
}
