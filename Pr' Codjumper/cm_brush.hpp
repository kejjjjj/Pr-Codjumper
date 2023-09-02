#pragma once


#include "pch.hpp"

void CM_BuildAxialPlanes(float(*planes)[6][4], cbrush_t* brush);
bool IntersectPlanes(const float(*planes)[4], float* xyz);

void CM_GetPolys(cbrush_t* brush);
int CM_ForEachBrushPlaneIntersection(const cbrush_t* brush, const float(*axialPlanes)[4], ShowCollisionBrushPt* brushPts);
void CM_GetPlaneVec4Form(const cbrushside_t* sides, const float(*axialPlanes)[4], int index, float* expandedPlane);
int CM_AddSimpleBrushPoint(const cbrush_t* brush, const float(*axialPlanes)[4], const __int16* side_indices, const float* xyz, int pt_count, ShowCollisionBrushPt* brush_pts);
bool CM_BuildBrushWindingForSide(winding_t* winding, float* planeNormal, int sideIndex, const ShowCollisionBrushPt* pts, int ptCount);
void CM_AddExteriorPointToWindingProjected(winding_t* w, const float* pt, int i, int j);
float CM_RepresentativeTriangleFromWinding(const winding_t* w, const float* normal, int* i0, int* i1, int* i2);
bool PlaneFromPoints(vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c);
void CM_ReverseWinding(winding_t* w);
void CM_AddColinearExteriorPointToWindingProjected(winding_t* w, const float* pt, int i, int j, int index0, int index1);
bool VecNCompareCustomEpsilon(const float* v0, const float* v1, float epsilon, int coordCount);
int CM_GetXyzList(int sideIndex, const ShowCollisionBrushPt* pts, int ptCount, float(*xyz)[3], int xyzLimit);
void CM_PickProjectionAxes(const float* normal, int* i, int* j);

inline std::list<winding_t> brushWindings;

inline winding_t winding_pool[256];