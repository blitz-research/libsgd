#pragma once

#include "geom/exports.h"

namespace sgd {

inline Signal<> appIniting;
inline Signal<> appExiting;

inline Signal<> appSuspending;
inline Signal<> appResuming;

inline Signal<> beginPollEvents;
inline Signal<> endPollEvents;

Map<String, Signal<CString>>& configVarChanged();

void initApp();
void exitApp();

void setConfigVar(CString name, CString value);
String getConfigVar(CString name);

void suspendApp();
void resumeApp();

void pollEvents();

Vec2u desktopSize();

} // namespace sgd
