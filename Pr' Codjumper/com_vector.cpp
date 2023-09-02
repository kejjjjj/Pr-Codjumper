#include "pch.hpp"


void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross) {
	cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
	cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
	cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

vec_t VectorLength(const vec3_t v) {
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

vec_t VectorLengthSquared(const vec3_t v) {
	return (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

vec_t Distance(const vec3_t p1, const vec3_t p2) {
	vec3_t v;

	VectorSubtract(p2, p1, v);
	return VectorLength(v);
}

vec_t DistanceSquared(const vec3_t p1, const vec3_t p2) {
	vec3_t v;

	VectorSubtract(p2, p1, v);
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}


void VectorInverse(vec3_t v) {
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

int VectorCompare(const vec3_t v1, const vec3_t v2) {
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2]) {
		return 0;
	}

	return 1;
}


vec_t VectorNormalize(vec3_t v) {
	float length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt(length);

	if (length) {
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}

void MatrixTransformVector43(const float* in1, const float(*in2)[3], float* out)
{
	*out = ((*in1 * (*in2)[0]) + (((*in2)[6] * in1[2]) + ((*in2)[3] * in1[1]))) + (*in2)[9];
	out[1] = (((*in2)[4] * in1[1]) + (((*in2)[1] * *in1) + ((*in2)[7] * in1[2]))) + (*in2)[10];
	out[2] = (((*in2)[5] * in1[1]) + (((*in2)[2] * *in1) + ((*in2)[8] * in1[2]))) + (*in2)[11];
}

float random(const float range) { //0 -> HI
	std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_real_distribution num{ 0.f, range };
	return num(mt);

}
float random(const float min, const float range) { //LO -> HI
	std::random_device rd;
	static std::mt19937 mt;
	std::uniform_real_distribution num{ min, range };
	return num(mt);
}