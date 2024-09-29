#pragma once

#include "signal.h" // NOLINT (header name)
#include "signal_impl.inl"
#include "std.h"

namespace sgd {

Signal<CString>& configVarChanged(CString name);

void setConfigVar(CString name, CString value);

String getConfigVar(CString name, CString orvalue = {});

bool configVarExists(CString name);

// clang-format off
#define SGD_BOOL_CONFIG_VAR(IDENT, NAME, DEFAULT) \
bool IDENT = DEFAULT; \
auto IDENT##_init = configVarChanged(NAME).connect(nullptr, [](CString value) { IDENT = truthiness(value); });
// clang-format on

} // namespace sgd
