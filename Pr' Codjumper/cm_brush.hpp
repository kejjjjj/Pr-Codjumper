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
bool VecNCompareCustomEpsilon(const float* v0, const float* v1, float epsilon, int coordCount);
int CM_GetXyzList(int sideIndex, const ShowCollisionBrushPt* pts, int ptCount, float(*xyz)[3], int xyzLimit);
void CM_PickProjectionAxes(const float* normal, int* i, int* j);
inline ShowCollisionBrushPt pts[1027];
inline int numColPoints = 0;