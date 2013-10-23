package brutus.compiler.report;

/**
 *
 */
public abstract class Report {
  private final Position position;

  protected Report(final Position position) {
    this.position = position;
  }

  public Position position() {
    return position;
  }

  public abstract int level();
}
