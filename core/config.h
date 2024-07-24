#pragma once

#include "signal.h" // NOLINT (header name)
#include "signal_impl.inl"
#include "std.h"

namespace sgd {

Map<String, Signal<CString>>& configVarChanged();

void setConfigVar(CString name, CString value);
String getConfigVar(CString name);

} // namespace sgd
