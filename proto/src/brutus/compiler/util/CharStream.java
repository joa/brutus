package brutus.compiler.util;

import java.io.IOException;
import java.io.Reader;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.function.IntConsumer;

/**
 *
 */
public interface CharStream {
  public boolean hasNext();

  public char next();

  public static CharStream of(final String string) {
    return new StringCharStream(string);
  }

  public static CharStream of(final Path path) {
    try {
      return new ReaderStream(
          Files.newBufferedReader(path, StandardCharsets.UTF_8));
    } catch(final IOException ioException) {
      return EmptyStream.INSTANCE;
    }
  }

  public static class StringCharStream implements CharStream {
    private final String string;
    private final int length;
    private int index = 0;

    private StringCharStream(final String string) {
      this.string = string;
      this.length = string.length();
    }

    @Override
    public boolean hasNext() {
      return index < length;
    }

    @Override
    public char next() {
      return string.charAt(index++);
    }
  }

  public static class ReaderStream implements CharStream {
    private final Reader reader;

    private char bufferedChar = '\0';

    private ReaderStream(final Reader reader) {
      this.reader = reader;
      next();
    }

    @Override
    public boolean hasNext() {
      return bufferedChar != '\0';
    }

    @Override
    public char next() {
      final char result = bufferedChar;

      try {
        bufferedChar = (char)reader.read();
      } catch(final IOException ioException) {
        bufferedChar = '\0';
        try {
          reader.close();
        } catch(final IOException ignored) {
          /* ignored */
        }
      }

      return result;
    }
  }

  public static enum EmptyStream implements CharStream {
    INSTANCE;

    @Override
    public boolean hasNext() {
      return false;
    }

    @Override
    public char next() {
      throw new UnsupportedOperationException();
    }
  }
}
