#pragma once

#include "pch.hpp"

int VectorCompare(const vec3_t v1, const vec3_t v2);
vec_t VectorLength(const vec3_t v);
vec_t VectorLengthSquared(const vec3_t v);
vec_t Distance(const vec3_t p1, const vec3_t p2);
vec_t DistanceSquared(const vec3_t p1, const vec3_t p2);
void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
vec_t VectorNormalize(vec3_t v);       // returns vector length
void VectorNormalizeFast(vec3_t v);     // does NOT return vector length, uses rsqrt approximation
vec_t VectorNormalize2(const vec3_t v, vec3_t out);
void VectorInverse(vec3_t v);

void MatrixTransformVector43(const float* in1, const float(*in2)[3], float* out);

float random(const float range); //0 -> HI
float random(const float min, const float range); //LO -> HI