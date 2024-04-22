#pragma once

#include "geom/exports.h"

namespace sgd {

inline Signal<> appIniting;

inline Signal<> beginPollEvents;
inline Signal<> endPollEvents;
inline Signal<> appSuspended;
inline Signal<> appResumed;

void initApp();

void suspendApp();
void resumeApp();

void pollEvents();

Vec2u desktopSize();

} // namespace sgd
