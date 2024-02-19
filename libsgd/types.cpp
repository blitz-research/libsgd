#include "types.h"

#include "internal.h"

void sgd_Release(SGD_Handle handle) {
	sgd::destroyHandle(handle);
}
