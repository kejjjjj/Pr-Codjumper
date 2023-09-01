#include "pch.hpp"


void CM_BuildAxialPlanes(float(*planes)[6][4], cbrush_t* brush)
{

	(*planes)[0][0] = -1.0;
	(*planes)[0][1] = 0.0;
	(*planes)[0][2] = 0.0;
	(*planes)[0][3] = -brush->mins[0];
	(*planes)[1][0] = 1.0;
	(*planes)[1][1] = 0.0;
	(*planes)[1][2] = 0.0;
	(*planes)[1][3] = brush->maxs[0];
	(*planes)[2][0] = 0.0;
	(*planes)[2][2] = 0.0;
	(*planes)[2][1] = -1.0;
	(*planes)[2][3] = -brush->mins[1];
	(*planes)[3][0] = 0.0;
	(*planes)[3][2] = 0.0;
	(*planes)[3][1] = 1.0;
	(*planes)[3][3] = brush->maxs[1];
	(*planes)[4][0] = 0.0;
	(*planes)[4][1] = 0.0;
	(*planes)[4][2] = -1.0;
	(*planes)[4][3] = -brush->mins[2];
	(*planes)[5][0] = 0.0;
	(*planes)[5][1] = 0.0;
	(*planes)[5][2] = 1.0;
	(*planes)[5][3] = brush->maxs[2];
}
bool IntersectPlanes(const float(*planes)[4], float* xyz)
{
	const float* plane2 = planes[2];
	const float* plane0 = planes[0];
	const float* plane1 = planes[1]; 

	float determinant = (((plane1[1] * plane2[2]) - (plane2[1] * plane1[2])) * plane0[0])
		+ (((plane2[1] * plane0[2]) - (plane0[1] * plane2[2])) * plane1[0])
		+ (((plane0[1] * plane1[2]) - (plane1[1] * plane0[2])) * plane2[0]);


	if (fabs(determinant) < 0.001f)
		return 0;

	determinant = 1.0f / determinant;

	xyz[0] = ((((plane1[1] * plane2[2]) - (plane2[1] * plane1[2])) * plane0[3])
		+ (((plane2[1] * plane0[2]) - (plane0[1] * plane2[2])) * plane1[3])
		+ (((plane0[1] * plane1[2]) - (plane1[1] * plane0[2])) * plane2[3])) * determinant;

	xyz[1] = ((((plane1[2] * plane2[0]) - (plane2[2] * plane1[0])) * plane0[3])
		+ (((plane2[2] * plane0[0]) - (plane0[2] * plane2[0])) * plane1[3])
		+ (((plane0[2] * plane1[0]) - (plane1[2] * plane0[0])) * plane2[3])) * determinant;

	xyz[2] = ((((plane1[0] * plane2[1]) - (plane2[0] * plane1[1])) * plane0[3])
		+ (((plane2[0] * plane0[1]) - (plane0[0] * plane2[1])) * plane1[3])
		+ (((plane0[0] * plane1[1]) - (plane1[0] * plane0[1])) * plane2[3])) * determinant;


	return true;
}
void CM_GetPlaneVec4Form(const cbrushside_t* sides, const float(*axialPlanes)[4], int index, float* expandedPlane)
{
	if (index >= 6) {
		cplane_s* plane = sides[index - 6].plane;

		expandedPlane[0] = plane->normal[0];
		expandedPlane[1] = plane->normal[1];
		expandedPlane[2] = plane->normal[2];
		expandedPlane[3] = plane->dist;
		return;
	}

	const float* plane = axialPlanes[index];

	*expandedPlane = plane[0];
	expandedPlane[1] = plane[1];
	expandedPlane[2] = plane[2];
	expandedPlane[3] = plane[3];

}
int CM_ForEachBrushPlaneIntersection(const cbrush_t* brush, const float(*axialPlanes)[4], ShowCollisionBrushPt* brushPts)
{
	int brushPoint = 0;
	cbrushside_t* brushside = brush->sides;

	int side6 = brush->numsides + 6;
	int prev6 = side6 - 1;
	int side4 = brush->numsides + 4;
	int prevPlane = side6 - 1;
	int sideIndex = 0;
	int nextSide = sideIndex + 1;
	int nextSide2 = nextSide + 1;
	int nextSide3 = nextSide2 + 1;
	int points = 0;
	float expanded_plane[3][4]{};
	short sideIdx[3] = {0,0,0};

	if (side4 > 0) {

		prevPlane = side6 - 1;
		sideIndex = 0;

		do {

			CM_GetPlaneVec4Form(brushside, axialPlanes, sideIndex, expanded_plane[0]);
			nextSide = sideIndex + 1;
			nextSide2 = (sideIndex + 1);

			for (sideIdx[1] = sideIndex + 1;
				nextSide2 < prev6;
				sideIdx[1] = nextSide2)
			{
				if (sideIndex < 6 || nextSide2 < 6 || brushside[sideIndex - 6].plane != brushside[nextSide2 - 6].plane)
				{
					CM_GetPlaneVec4Form(brushside, axialPlanes, nextSide2, expanded_plane[1]);
					nextSide3 = (nextSide2 + 1);
					for (sideIdx[2] = nextSide2 + 1;
						nextSide3 < side6;
						sideIdx[2] = nextSide3)
					{
						if ((sideIndex < 6 || nextSide3 < 6 || brushside[sideIndex - 6].plane != brushside[nextSide3 - 6].plane) && (nextSide2 < 6 || nextSide3 < 6 || brushside[nextSide2 - 6].plane != brushside[nextSide3 - 6].plane))
						{
							float out_pts[3];
							CM_GetPlaneVec4Form(brushside, axialPlanes, nextSide3, expanded_plane[2]);

							if (IntersectPlanes(expanded_plane, out_pts)) {

								points = CM_AddSimpleBrushPoint(brush, axialPlanes, sideIdx, out_pts, points, brushPts);

							}

						}
						nextSide3++;
					}
				}
				nextSide2++;
			}
			sideIndex = nextSide;
			sideIdx[0] = nextSide;


		} while (nextSide < side4);

	}

	return points;

}
int CM_AddSimpleBrushPoint(const cbrush_t* brush, const float(*axialPlanes)[4], const __int16* sideIndices, const float* xyz, int pt_count, ShowCollisionBrushPt* brushPoints)
{
	int sIndex = 0;
	do
	{

		if (axialPlanes[sIndex][0] * xyz[0] + axialPlanes[sIndex][1] * xyz[1] + axialPlanes[sIndex][2] * xyz[2] - axialPlanes[sIndex][3] > 0.1f)
			return pt_count;


	} while (++sIndex < 6);


	for (int i = 0; i < brush->numsides; i++) {

		const cplane_s* plane = brush->sides[i].plane;

		if (plane != brush->sides[sideIndices[0] - 6].plane
			&& plane != brush->sides[sideIndices[1] - 6].plane
			&& plane != brush->sides[sideIndices[2] - 6].plane
			&& ((plane->normal[0] * xyz[0]) + (plane->normal[1] * xyz[1]) + (plane->normal[2] * xyz[2]) - plane->dist) > 0.1f)
			return pt_count;
		

	}

	if (pt_count >= 1024) {
		Com_Printf(CON_CHANNEL_OBITUARY, "More than %i points from plane intersections on %i-sided brush\n", pt_count, brush->numsides);
		return pt_count;
	}


	//std::cout << "brushPoints[" << pt_count << "].xyz[0] = " << xyz[0] << '\n';
	//std::cout << "brushPoints[" << pt_count << "].xyz[1] = " << xyz[1] << '\n';
	//std::cout << "brushPoints[" << pt_count << "].xyz[2] = " << xyz[2] << '\n';

	brushPoints[pt_count].xyz[0] = *xyz;
	brushPoints[pt_count].xyz[1] = xyz[1];
	brushPoints[pt_count].xyz[2] = xyz[2];
	brushPoints[pt_count].sideIndex[0] = *sideIndices;
	brushPoints[pt_count].sideIndex[1] = sideIndices[1];
	brushPoints[pt_count].sideIndex[2] = sideIndices[2];

	return pt_count+1;
}
void CM_AddExteriorPointToWindingProjected(winding_t* w, const float* pt, int i, int j)
{
	const auto cross_p = [](const float* pt0, const float* pt1, const float* pt2, const int i, const int j) -> float
	{
		return (pt2[j] - pt1[j]) * pt0[i] + (pt0[j] - pt2[j]) * pt1[i] + (pt1[j] - pt0[j]) * pt2[i];
	};

	int bestIndex = -1;
	float max = FLT_MAX;
	float best = 0;
	int prev = w->numpoints - 1;

	std::cout << "numpoints: " << w->numpoints << '\n';

	for (int o = 0; o < w->numpoints; o++) {
		
		const float a = cross_p(w->p[prev], pt, w->p[o], i, j);

		if (a > best) {
			best = a;
			bestIndex = o;
		}

		prev = o;
	}

	if (best < -0.001f) {

		memmove((char*)w->p[bestIndex + 1], (char*)w->p[bestIndex], 12 * (w->numpoints - bestIndex));
		VectorCopy(pt, w->p[bestIndex]);

		++w->numpoints;
	}

	else if (best <= 0.001f) {

	}

}
bool CM_BuildBrushWindingForSide(winding_t* winding, float* planeNormal, int sideIndex, const ShowCollisionBrushPt* pts, int ptCount)
{
	float points[1024][3]{};

	int numWindings = CM_GetXyzList(sideIndex, pts, ptCount, points, ptCount);

	//std::cout << "numWindings: " << numWindings << '\n';

	if (numWindings < 3)
		return 0;

	int i = 0, j = 0;

	CM_PickProjectionAxes(planeNormal, &i, &j);

	VectorCopy(points[0], winding->p[0]);
	VectorCopy(points[1], winding->p[1]);
	winding->numpoints = 2;

	int windings = numWindings - 2;

	for (int a = windings; a > 0; a--) {

		CM_AddExteriorPointToWindingProjected(winding, points[a], i, j);

	}

	return 0;
}
bool VecNCompareCustomEpsilon(const float* v0, const float* v1, float epsilon, int coordCount)
{
	int i;
	int sub; 
	double v5; 
	float v7; 

	i = 0;
	sub = v0 - v1;
	while (1)
	{
		v5 = *(v1 + sub) - *v1;
		v7 = epsilon * epsilon;
		if (v5 * v5 > v7)
		{
			break;
		}
		++i;
		++v1;
		if (i >= coordCount)
		{
			return 1;
		}
	}
	return 0;
}
int CM_GetXyzList(int sideIndex, const ShowCollisionBrushPt* pts, int ptCount, float(*xyz)[3], int xyzLimit)
{

	const auto CM_PointInList = [](const float* point, float(*xyzList)[3], int xyzCount) -> bool {

		int points = 0;
		if (xyzCount <= 0)
			return 0;
		while (!VecNCompareCustomEpsilon((const float*)xyzList[points], point, 0.1, xyzCount))
		{
			++points;
			++xyzList;
			if (points >= xyzCount)
				return 0;
		}
		return 1;

	};

	int windingPoints = 0;
	
	if (!ptCount)
		return 0;

	for (int i = 0; i < ptCount; i++) {
		if ((pts[i].sideIndex[0] == sideIndex || pts[i].sideIndex[1] == sideIndex || pts[i].sideIndex[2] == sideIndex) && !CM_PointInList(pts[i].xyz, xyz, windingPoints)) {
			if (windingPoints == xyzLimit)
			{
				Com_Printf(CON_CHANNEL_OBITUARY,"^1Winding point limit (%i) exceeded on brush face\n");
			}


			VectorCopy(pts[i].xyz, xyz[windingPoints]);

			//std::cout << "xyz[" << windingPoints << "][0]: " << xyz[windingPoints][0] << '\n';
			//std::cout << "xyz[" << windingPoints << "][1]: " << xyz[windingPoints][1] << '\n';
			//std::cout << "xyz[" << windingPoints << "][2]: " << xyz[windingPoints][2] << '\n';

			++windingPoints;
		}

	}

	return windingPoints;
}
void CM_PickProjectionAxes(const float* normal, int* i, int* j)
{
	int k = 0;

	if (fabs(normal[1]) > fabs(normal[0]))
	{
		k = 1;
	}

	if (fabs(normal[2]) > fabs(normal[k]))
	{
		k = 2;
	}

	*i = ~k & 1;
	*j = ~k & 2;
}
void CM_GetPolys(cbrush_t* brush)
{
	float axialPlanes[24];
	winding_t winding = {};


	CM_BuildAxialPlanes((float(*)[6][4])axialPlanes, brush);

	numColPoints = CM_ForEachBrushPlaneIntersection(brush, (float(*)[4])axialPlanes, pts);

	int side = 0;
	do {
		if (CM_BuildBrushWindingForSide(&winding, (axialPlanes + side * 4), side, pts, numColPoints)) {
			
		}
	} while (++side < 6);


	for (auto i = 0; i < numColPoints; i++)
		Com_Printf(CON_CHANNEL_OBITUARY, std::format("[^2{}^7] = [^2{}, {}, {}^7]\n", i, pts[i].xyz[0], pts[i].xyz[1], pts[i].xyz[2]).c_str());



}