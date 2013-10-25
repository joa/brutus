package brutus.compiler.symbol;

import brutus.compiler.name.Name;
import brutus.compiler.name.NameTable;

/**
 *
 */
public final class SymbolTable {
  public final Symbol root;

  private final NameTable names;

  public SymbolTable(final NameTable names) {
    this.names = names;
    this.root = new RootSymbol(names.predef_empty, this);
  }

  public Symbol get(final Name name) {
    int offset0 = name.offset;
    int offset1 = name.indexOf('.');
    char[] chars = name.chars;
    Symbol symbol = root;

    while(offset1 != -1) {
      final Name slice = names.get(chars, offset0, (offset1 - offset0));
      symbol = symbol.scope().get(slice);
      if(symbol.isError()) {
        return symbol;
      }
      offset0 = offset1 + 1;
      offset1 = name.indexOf('.', offset0);
    }

    return symbol.scope().get(
        names.get(chars, offset0, name.count - offset0));
  }

  public Symbol error()                 { return new ErrorSymbol(names.predef_empty, root); }
  public Symbol error(final Name name)  { return new ErrorSymbol(name, root); }

  public Symbol predef_brutus_Atom   () { return get(names.predef_brutus_Atom   ); }
  public Symbol predef_brutus_Object () { return get(names.predef_brutus_Object ); }
  public Symbol predef_brutus_Unit   () { return get(names.predef_brutus_Unit   ); }
  public Symbol predef_brutus_Boolean() { return get(names.predef_brutus_Boolean); }
  public Symbol predef_brutus_String () { return get(names.predef_brutus_String ); }

  public Symbol predef_brutus_numeric(final int kind)   { return get(names.predef_numeric(kind));   }
  public Symbol predef_brutus_function(final int arity) { return get(names.predef_function(arity)); }
}
