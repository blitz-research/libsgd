#pragma once

#include "std.h"

namespace sgd {

// I really wanted to enable these only for 'unsigned' enums, but
// std::underlying_type always seems to return an unsigned type, so you get
// these for ALL enums until I explore further.
#define BITMASK_OP template <class EnumTy, class = std::enable_if<std::is_enum<EnumTy>::value>>

BITMASK_OP EnumTy operator&(EnumTy x, EnumTy y) { return EnumTy(int(x) & int(y)); }
BITMASK_OP EnumTy operator|(EnumTy x, EnumTy y) { return EnumTy(int(x) | int(y)); }
BITMASK_OP EnumTy operator^(EnumTy x, EnumTy y) { return EnumTy(int(x) ^ int(y)); }
BITMASK_OP EnumTy& operator&=(EnumTy& x, EnumTy y) { return x = EnumTy(x & y); }
BITMASK_OP EnumTy& operator|=(EnumTy& x, EnumTy y) { return x = EnumTy(x | y); }
BITMASK_OP EnumTy& operator^=(EnumTy& x, EnumTy y) { return x = EnumTy(x ^ y); }
BITMASK_OP EnumTy operator~(EnumTy x) { return EnumTy(~int(x)); }

} // namespace xros
