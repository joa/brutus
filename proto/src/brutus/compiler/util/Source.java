package brutus.compiler.util;

import java.nio.file.Path;

/**
 *
 */
public interface Source {
  public CharStream charStream();

  public String name();

  public static Source of(final Path path) {
    return new FileSource(path);
  }

  public static class FileSource implements Source {
    private final Path path;

    private FileSource(final Path path) {
      this.path = path;
    }

    @Override
    public CharStream charStream() {
      return CharStream.of(path);
    }

    @Override
    public String name() {
      return path.getFileName().toString();
    }
  }
}
