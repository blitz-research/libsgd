#pragma once

#include "geom/exports.h"

namespace sgd {

inline Signal<> appIniting;
inline Signal<> appExiting;

inline Signal<> appSuspending;
inline Signal<> appResuming;

inline Signal<> beginPollEvents;
inline Signal<> endPollEvents;

void initApp();
void exitApp();

void suspendApp();
void resumeApp();

void pollEvents();

Vec2u desktopSize();

} // namespace sgd
