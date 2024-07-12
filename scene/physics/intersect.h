#pragma once

#include "contact.h"

namespace sgd {

constexpr auto degenerateNormalEpsilon = (real).000001;

//! ray must be normalized
bool intersectRaySphere(CLiner ray, CVec3r v0, real radius, Contact& contact);

//! ray and dir must be normalized
bool intersectRayCapsule(CLiner ray, CVec3r v0, CVec3r dir, real length, real radius, Contact& contact);

//! ray and dir must be normalized
bool intersectRayCylinder(CLiner ray, CVec3r v0, CVec3r dir, real length, Contact& contact);

//! ray must be normalized
bool intersectRayEdge(CLiner ray, real radius, CVec3r v0, CVec3r v1, Contact& contact);

//! ray must be normalized
bool intersectRayTriangle(CLiner ray, real radius, CVec3r v0, CVec3r v1, CVec3r v2, Contact& contact);

} // namespace sgf
