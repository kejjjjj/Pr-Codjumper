#pragma once
#include "pch.hpp"

struct range_t
{
	float x1;
	float x2;
	bool split;
};

void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
float ProjectionX(float angle, float fov);
range_t AnglesToRange(float start, float end, float yaw, float fov);

float AngleNormalizePI(float angle);
float AngleNormalize360(float angle);
float AngleNormalize180(float angle);
float AngleNormalize90(float angle);

float AngleDelta(float angle1, float angle2);

float AngularDistance(float value1, float value2);

std::optional<float> CG_GetOptYawDelta(pmove_t* pm, pml_t* pml);