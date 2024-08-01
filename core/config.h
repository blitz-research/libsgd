#pragma once

#include "signal.h" // NOLINT (header name)
#include "signal_impl.inl"
#include "std.h"

namespace sgd {

Signal<CString>& configVarChanged(CString name);

void setConfigVar(CString name, CString value);
String getConfigVar(CString name);

} // namespace sgd
