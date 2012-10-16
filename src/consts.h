#ifndef BRUTUS_CONSTS_H_
#define BRUTUS_CONSTS_H_
namespace brutus {
  namespace consts {
    static const int Byte = 1;
    static const int KiloByte = Byte * 1024;
    static const int MegaByte = KiloByte * 1024;
    static const int GigaByte = MegaByte * 1024;

    static const int Alignment = 8; //TODO(joa): jaja alignment check arch
    static const int PageSize = 4 * KiloByte; //TODO(joa): also assuming 4kb page size

    static const int MaxTupleArity = 32;
    static const int MaxFunctionArity = 32;
  }
}
#endif
