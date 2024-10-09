#pragma once

#include <core/exports.h>

namespace sgd {

String serialize(Object *obj);

Object* deserialize(CString jsrc);

}
