#include "config.h"

#include <mutex>

namespace sgd {

//template struct Signal<CString>;

namespace {

Map<String, String> g_configVars;

Map<String, Signal<CString>>* g_configVarChanged;

std::mutex g_configMutex;

} // namespace

Map<String, Signal<CString>>& configVarChanged() {

	if (!g_configVarChanged) g_configVarChanged = new Map<String, Signal<CString>>();

	return *g_configVarChanged;
}

void setConfigVar(CString name, CString value) {

	std::lock_guard<std::mutex> lock(g_configMutex);

	auto* p = &g_configVars[name];
	if (value == *p) return;

	*p = value;
	configVarChanged()[name].emit(value);
}

String getConfigVar(CString name) {

	std::lock_guard<std::mutex> lock(g_configMutex);

	return g_configVars[name];
}

} // namespace sgd
