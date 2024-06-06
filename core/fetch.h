#pragma once

#include "expected.h"
#include "fileio.h"
#include "std.h"

namespace sgd {

struct StreamBuffer;

Expected<String, FileioEx> fetchString(CString url);

Expected<Data, FileioEx> fetchData(CString url);

} // namespace sgd
