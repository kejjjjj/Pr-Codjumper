#pragma once

#include "pch.hpp"

void Geom_CreateBall(const fvec3& ref_org, const float radius, const int32_t latitudeSegments, const int32_t longitudeSegments, std::list<fvec3>& points);