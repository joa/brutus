module brutus { 
  class Atom[T] {
    virtual def hashCode: Int32
    virtual def toString: String
  }
  
  immutable class Unit extends Atom[T] {
    override def hashCode = -1
    override def toString = "()"
  }

  immutable class Int8 {
    native def hashCode
    native def toString
    native def ~: Int8

    native def +(that: Int8): Int8
    native def -(that: Int8): Int8
    native def *(that: Int8): Int8
    native def /(that: Int8): Int8
    native def %(that: Int8): Int8
    native def &(that: Int8): Int8
    native def |(that: Int8): Int8
    native def ^(that: Int8): Int8
    native def >>(that: Int8): Int8
    native def <<(that: Int8): Int8

    native def +(that: Int16): Int16
    native def -(that: Int16): Int16
    native def *(that: Int16): Int16
    native def /(that: Int16): Int16
    native def %(that: Int16): Int16
    native def &(that: Int16): Int16
    native def |(that: Int16): Int16
    native def ^(that: Int16): Int16
    native def >>(that: Int16): Int16
    native def <<(that: Int16): Int16

    native def +(that: Int32): Int32
    native def -(that: Int32): Int32
    native def *(that: Int32): Int32
    native def /(that: Int32): Int32
    native def %(that: Int32): Int32
    native def &(that: Int32): Int32
    native def |(that: Int32): Int32
    native def ^(that: Int32): Int32
    native def >>(that: Int32): Int32
    native def <<(that: Int32): Int32

    native def +(that: Int64): Int64
    native def -(that: Int64): Int64
    native def *(that: Int64): Int64
    native def /(that: Int64): Int64
    native def %(that: Int64): Int64
    native def &(that: Int64): Int64
    native def |(that: Int64): Int64
    native def ^(that: Int64): Int64
    native def >>(that: Int64): Int64
    native def <<(that: Int64): Int64

    native def +(that: UInt8): Int8
    native def -(that: UInt8): Int8
    native def *(that: UInt8): Int8
    native def /(that: UInt8): Int8
    native def %(that: UInt8): Int8
    native def &(that: UInt8): Int8
    native def |(that: UInt8): Int8
    native def ^(that: UInt8): Int8
    native def >>(that: UInt8): Int8
    native def <<(that: UInt8): Int8

    native def +(that: UInt16): Int16
    native def -(that: UInt16): Int16
    native def *(that: UInt16): Int16
    native def /(that: UInt16): Int16
    native def %(that: UInt16): Int16
    native def &(that: UInt16): Int16
    native def |(that: UInt16): Int16
    native def ^(that: UInt16): Int16
    native def >>(that: UInt16): Int16
    native def <<(that: UInt16): Int16

    native def +(that: UInt32): Int32
    native def -(that: UInt32): Int32
    native def *(that: UInt32): Int32
    native def /(that: UInt32): Int32
    native def %(that: UInt32): Int32
    native def &(that: UInt32): Int32
    native def |(that: UInt32): Int32
    native def ^(that: UInt32): Int32
    native def >>(that: UInt32): Int32
    native def <<(that: UInt32): Int32

    native def +(that: UInt64): Int64
    native def -(that: UInt64): Int64
    native def *(that: UInt64): Int64
    native def /(that: UInt64): Int64
    native def %(that: UInt64): Int64
    native def &(that: UInt64): Int64
    native def |(that: UInt64): Int64
    native def ^(that: UInt64): Int64
    native def >>(that: UInt64): Int64
    native def <<(that: UInt64): Int64
  }

  class Int16 {
  }

  class Int32 {
  }

  class Int64 {
  }

  class UInt8 {
  }

  class UInt16 {
  }

  class UInt32 {
  }

  class UInt64 {
  }

  class Float32 {
  }

  class Float64 {
  }

  class Bool {
  }

  class Char {
  }

  class Iterator[T] {
    def next(): T
    def hasNext(): Bool
    def remove(): Unit
  }

  class Iterable[T] {
  }

  class Array[T] {
    native def apply(index: Int): T
    native def update(index: Int, value: T): Unit
    native def slice(offset: Int, count: Int): Array[T]
  }

  class String {
  }
}
