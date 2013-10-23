module brutus {
  virtual class Atom {
    virtual native idempotent def ==(that: Atom): Boolean
    idempotent def !=(that: Atom): Boolean = (this == that).inverse

    virtual def hashCode: Int32
    virtual def toString: String
  }

  native idempotent def instanceOf[T](value: Atom): Boolean
  pure def not(value: Boolean): Boolean = value.inverse

  immutable class Unit {
    override def hashCode = -1
    override def toString = ""
  }

  immutable class String {
    override def toString: String = this
  }

  virtual immutable sealed class Boolean {
    pure def inverse: Boolean =
      if {
        this == boolean.TRUE  -> boolean.FALSE
        this == boolean.FALSE -> boolean.TRUE
      }

    def choose[T](ifTrue: T, ifFalse: T): T =
      if {
        this -> ifTrue
           _ -> ifFalse
      }
  }

  module boolean {
    val TRUE = new True()
    val FALSE = new Boolean()

    class True : Boolean {
      override def hashCode = 1
      override def toString = "true"
    }

    class False : Boolean {
      override def hashCode = 0
      override def toString = "false"
    }
  }

  class Numeric {
    pure def toInt8   : Int8
    pure def toInt16  : Int16
    pure def toInt32  : Int32
    pure def toInt64  : Int64
    pure def toUInt8  : UInt8
    pure def toUInt16 : UInt16
    pure def toUInt32 : UInt32
    pure def toUInt64 : UInt64
    pure def toFloat32: Float32
    pure def toFloat64: Float64
  }

  immutable class Int32 : Numeric {
    pure def +(that: Int8): Int32 = this + that.toInt32
    pure def -(that: Int8): Int32 = this - that.toInt32
    pure def *(that: Int8): Int32 = this * that.toInt32
    pure def /(that: Int8): Int32 = this / that.toInt32
    pure def %(that: Int8): Int32 = this % that.toInt32
    
    pure def +(that: Int16): Int32 = this + that.toInt32
    pure def -(that: Int16): Int32 = this - that.toInt32
    pure def *(that: Int16): Int32 = this * that.toInt32
    pure def /(that: Int16): Int32 = this / that.toInt32
    pure def %(that: Int16): Int32 = this % that.toInt32
    
    native pure def +(that: Int32): Int32
    native pure def -(that: Int32): Int32
    native pure def *(that: Int32): Int32
    native pure def /(that: Int32): Int32
    native pure def %(that: Int32): Int32

    pure def +(that: Int64): Int64 = this.toInt64 + that
    pure def -(that: Int64): Int64 = this.toInt64 - that
    pure def *(that: Int64): Int64 = this.toInt64 * that
    pure def /(that: Int64): Int64 = this.toInt64 / that
    pure def %(that: Int64): Int64 = this.toInt64 % that

    pure def +(that: Float32): Float32 = this.toFloat32 + that
    pure def -(that: Float32): Float32 = this.toFloat32 - that
    pure def *(that: Float32): Float32 = this.toFloat32 * that
    pure def /(that: Float32): Float32 = this.toFloat32 / that
    pure def %(that: Float32): Float32 = this.toFloat32 % that

    pure def +(that: Float64): Float64 = this.toFloat64 + that
    pure def -(that: Float64): Float64 = this.toFloat64 - that
    pure def *(that: Float64): Float64 = this.toFloat64 * that
    pure def /(that: Float64): Float64 = this.toFloat64 / that
    pure def %(that: Float64): Float64 = this.toFloat64 % that
    
    native pure def bitOr(that: Int32): Int32
    native pure def bitAnd(that: Int32): Int32
    native pure def bitXor(that: Int32): Int32
    native pure def bitShl(that: Int32): Int32
    native pure def bitShr(that: Int32): Int32
    native pure def bitNot: Int

    override native def toInt8
    override native def toInt16
    override def toInt32 = this
    override native def toInt64
    override native def toUInt8
    override native def toUInt16
    override native def toUInt32
    override native def toUInt64
    override native def toFloat32
    override native def toFloat64

    override native def hashCode
    override native def toString
  }
}
