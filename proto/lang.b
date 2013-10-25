module brutus {
  virtual class Atom {
    //replace with something like def[T -> this.type](that: T): Boolean
    virtual native idempotent def ==(that: Atom): Boolean
    idempotent def !=(that: Atom): Boolean = (this == that).inverse

    virtual def hashCode: Int32
    virtual def toString: String
  }

  virtual class Object : Atom

  sealed virtual class Value : Atom

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
    def fold[T](ifTrue: T, ifFalse: T): T = if {
      this == boolean.TRUE  -> ifTrue
      this == boolean.FALSE -> ifFalse
    }

    pure def inverse: Boolean = fold(boolean.FALSE, boolean.TRUE)

    pure def &(that: Boolean): Boolean = this.fold(that, false)
    pure def |(that: Boolean): Boolean = this.fold(true, that )
    pure def ^(that: Boolean): Boolean = this.fold(
                                           that.fold(false, true),
                                           that.fold(true, false))
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

  virtual immutable sealed class Numeric[T] : Value {
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

  immutable class Int8 : Numeric[Int8] {
    override        def toInt8 = this
    override native def toInt16
    override native def toInt32
    override native def toInt64
    override native def toUInt8
    override native def toUInt16
    override native def toUInt32
    override native def toUInt64
    override native def toFloat32
    override native def toFloat64
  }

  immutable class Int16 : Numeric[Int16] {
    override native def toInt8
    override        def toInt16 = this
    override native def toInt32
    override native def toInt64
    override native def toUInt8
    override native def toUInt16
    override native def toUInt32
    override native def toUInt64
    override native def toFloat32
    override native def toFloat64
  }

  immutable class Int32 : Numeric[Int32] {
    pure def + (that: Int8): Int32   = this +  that.toInt32
    pure def - (that: Int8): Int32   = this -  that.toInt32
    pure def * (that: Int8): Int32   = this *  that.toInt32
    pure def / (that: Int8): Int32   = this /  that.toInt32
    pure def % (that: Int8): Int32   = this %  that.toInt32
    pure def > (that: Int8): Boolean = this >  that.toInt32
    pure def < (that: Int8): Boolean = this <  that.toInt32
    pure def >=(that: Int8): Boolean = this >= that.toInt32
    pure def <=(that: Int8): Boolean = this >= that.toInt32
    
    pure def + (that: Int16): Int32   = this +  that.toInt32
    pure def - (that: Int16): Int32   = this -  that.toInt32
    pure def * (that: Int16): Int32   = this *  that.toInt32
    pure def / (that: Int16): Int32   = this /  that.toInt32
    pure def % (that: Int16): Int32   = this %  that.toInt32
    pure def > (that: Int16): Boolean = this >  that.toInt32
    pure def < (that: Int16): Boolean = this <  that.toInt32
    pure def >=(that: Int16): Boolean = this >= that.toInt32
    pure def <=(that: Int16): Boolean = this >= that.toInt32
    
    native pure def + (that: Int32): Int32
    native pure def - (that: Int32): Int32
    native pure def * (that: Int32): Int32
    native pure def / (that: Int32): Int32
    native pure def % (that: Int32): Int32
    native pure def > (that: Int32): Boolean
    native pure def < (that: Int32): Boolean
    native pure def >=(that: Int32): Boolean
    native pure def <=(that: Int32): Boolean

    pure def + (that: Int64): Int64   = this.toInt64 +  that
    pure def - (that: Int64): Int64   = this.toInt64 -  that
    pure def * (that: Int64): Int64   = this.toInt64 *  that
    pure def / (that: Int64): Int64   = this.toInt64 /  that
    pure def % (that: Int64): Int64   = this.toInt64 %  that
    pure def > (that: Int64): Boolean = this.toInt64 >  that
    pure def < (that: Int64): Boolean = this.toInt64 <  that
    pure def >=(that: Int64): Boolean = this.toInt64 >= that
    pure def <=(that: Int64): Boolean = this.toInt64 >= that

    pure def + (that: Float32): Float32 = this.toFloat32 +  that
    pure def - (that: Float32): Float32 = this.toFloat32 -  that
    pure def * (that: Float32): Float32 = this.toFloat32 *  that
    pure def / (that: Float32): Float32 = this.toFloat32 /  that
    pure def % (that: Float32): Float32 = this.toFloat32 %  that
    pure def > (that: Float32): Boolean = this.toFloat32 >  that
    pure def < (that: Float32): Boolean = this.toFloat32 <  that
    pure def >=(that: Float32): Boolean = this.toFloat32 >= that
    pure def <=(that: Float32): Boolean = this.toFloat32 >= that

    pure def + (that: Float64): Float64 = this.toFloat64 +  that
    pure def - (that: Float64): Float64 = this.toFloat64 -  that
    pure def * (that: Float64): Float64 = this.toFloat64 *  that
    pure def / (that: Float64): Float64 = this.toFloat64 /  that
    pure def % (that: Float64): Float64 = this.toFloat64 %  that
    pure def > (that: Float64): Boolean = this.toFloat64 >  that
    pure def < (that: Float64): Boolean = this.toFloat64 <  that
    pure def >=(that: Float64): Boolean = this.toFloat64 >= that
    pure def <=(that: Float64): Boolean = this.toFloat64 >= that
    
    native pure def bitOr (that: Int32): Int32
    native pure def bitAnd(that: Int32): Int32
    native pure def bitXor(that: Int32): Int32
    native pure def bitShl(that: Int32): Int32
    native pure def bitShr(that: Int32): Int32
    native pure def bitNot             : Int32

    override native def toInt8
    override native def toInt16
    override        def toInt32 = this
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

  immutable class Int64 : Numeric[Int64] {
    override native def toInt8
    override native def toInt16
    override native def toInt32
    override        def toInt64 = this
    override native def toUInt8
    override native def toUInt16
    override native def toUInt32
    override native def toUInt64
    override native def toFloat32
    override native def toFloat64
  }

  immutable class UInt8 : Numeric[UInt8] {
    override native def toInt8
    override native def toInt16
    override native def toInt32
    override native def toInt64
    override        def toUInt8 = this
    override native def toUInt16
    override native def toUInt32
    override native def toUInt64
    override native def toFloat32
    override native def toFloat64
  }

  immutable class UInt16 : Numeric[UInt16] {
    override native def toInt8
    override native def toInt16
    override native def toInt32
    override native def toInt64
    override native def toUInt8
    override        def toUInt16 = this
    override native def toUInt32
    override native def toUInt64
    override native def toFloat32
    override native def toFloat64
  }

  immutable class UInt32 : Numeric[UInt32] {
    override native def toInt8
    override native def toInt16
    override native def toInt32
    override native def toInt64
    override native def toUInt8
    override native def toUInt16
    override        def toUInt32 = this
    override native def toUInt64
    override native def toFloat32
    override native def toFloat64
  }

  immutable class UInt64 : Numeric[UInt64] {
    override native def toInt8
    override native def toInt16
    override native def toInt32
    override native def toInt64
    override native def toUInt8
    override native def toUInt16
    override native def toUInt32
    override        def toUInt64 = this
    override native def toFloat32
    override native def toFloat64
  }

  immutable class Float32 : Numeric[Float32] {
    override native def toInt8
    override native def toInt16
    override native def toInt32
    override native def toInt64
    override native def toUInt8
    override native def toUInt16
    override native def toUInt32
    override native def toUInt64
    override        def toFloat32 = this
    override native def toFloat64
  }

  immutable class Float64 : Numeric[Float64] {
    override native def toInt8
    override native def toInt16
    override native def toInt32
    override native def toInt64
    override native def toUInt8
    override native def toUInt16
    override native def toUInt32
    override native def toUInt64
    override native def toFloat32
    override        def toFloat64 = this
  }
}
