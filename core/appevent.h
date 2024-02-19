#pragma once

#include "std.h"

namespace sgd {

using AppEventFunc = Function<void()>;

void runOnMainThread(AppEventFunc func, bool sync);

void beginAppEventLoop();

void endAppEventLoop();

} // namespace sgd
