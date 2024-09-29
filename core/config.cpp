#include "config.h"

#include <mutex>

namespace sgd {

// template struct Signal<CString>;

namespace {

Map<String, String>* g_configVars;

Map<String, Signal<CString>>* g_configVarChanged;

std::mutex g_configMutex;

Map<String, String>& configVars() {
	if (!g_configVars) g_configVars = new Map<String, String>();

	return *g_configVars;
}

} // namespace

Signal<CString>& configVarChanged(CString name) {

	if (!g_configVarChanged) g_configVarChanged = new Map<String, Signal<CString>>();

	return (*g_configVarChanged)[name];
}

void setConfigVar(CString name, CString value) {

	std::lock_guard<std::mutex> lock(g_configMutex);

	auto* p = &configVars()[name];
	if (value == *p) return;

	*p = value;

	configVarChanged(name).emit(value);
}

String getConfigVar(CString name, CString orvalue) {

	std::lock_guard<std::mutex> lock(g_configMutex);

	auto it = configVars().find(name);
	if (it == configVars().end()) return orvalue;

	return it->second;
}

bool configVarExists(CString name) {

	std::lock_guard<std::mutex> lock(g_configMutex);

	return configVars().find(name) != configVars().end();
}

} // namespace sgd
