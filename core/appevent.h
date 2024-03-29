#pragma once

#include "std.h"

namespace sgd {

using AppEventFunc = Function<void()>;

void runOnMainThread(AppEventFunc func, bool sync = true);

void beginAppEventLoop();

void endAppEventLoop();

} // namespace sgd
