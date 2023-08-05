#pragma once
#include "pch.hpp"


float AngleNormalizePI(float angle);
float AngleNormalize360(float angle);
float AngleNormalize180(float angle);
float AngleDelta(float angle1, float angle2);

inline float opt_angle = 0;

std::optional<float> CG_GetOptYawDelta(pmove_t* pm, pml_t* pml);