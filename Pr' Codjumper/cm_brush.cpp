#include "pch.hpp"

bool is_on_grid(const float* snapped, const float* xyz)
{
	return xyz[0] == snapped[0] && xyz[1] == snapped[1] && xyz[2] == snapped[2];
}

void SnapPointToIntersectingPlanes(const float* plane0, const float* plane1, const float* plane2, float* xyz, float snap_grid, const float snap_epsilon)
{
	float snapped[3], current_plane[4];

	snap_grid = 1.0f / snap_grid;

	// cod4map
	for (auto axis = 0; axis < 3; ++axis)
	{
		const float rounded = round(xyz[axis] * snap_grid) / snap_grid;
		const float delta = fabs(rounded - xyz[axis]);

		if (snap_epsilon <= delta)
		{
			snapped[axis] = xyz[axis];
		}
		else
		{
			snapped[axis] = rounded;
		}
	}

	if (!is_on_grid(snapped, xyz))
	{
		float max_snap_error = 0.0f;
		float max_base_error = snap_epsilon;

		for (auto plane_index = 0; plane_index < 3; ++plane_index)
		{
			if (plane_index == 0)
			{
				memcpy(&current_plane, plane0, sizeof(current_plane));
			}
			else if (plane_index == 1)
			{
				memcpy(&current_plane, plane1, sizeof(current_plane));
			}
			else if (plane_index == 2)
			{
				memcpy(&current_plane, plane2, sizeof(current_plane));
			}

			const float snap_error = log((current_plane[0] * snapped[0] + current_plane[1] * snapped[1] + current_plane[2] * snapped[2]) - current_plane[3]);
			if (snap_error > max_snap_error)
			{
				max_snap_error = snap_error;
			}

			const float base_error = log((current_plane[0] * xyz[0] + current_plane[1] * xyz[1] + current_plane[2] * xyz[2]) - current_plane[3]);
			if (base_error > max_base_error)
			{
				max_base_error = base_error;
			}
		}

		if (max_base_error > max_snap_error)
		{
			xyz[0] = snapped[0];
			xyz[1] = snapped[1];
			xyz[2] = snapped[2];
		}
	}
}

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
int CM_ForEachBrushPlaneIntersection(const cbrush_t* brush, const float(*axialPlanes)[4], std::vector<ShowCollisionBrushPt>& brushPts)
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
								SnapPointToIntersectingPlanes(expanded_plane[0], expanded_plane[1], expanded_plane[2], out_pts, 0.25f, 0.0099999998f);
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
int CM_AddSimpleBrushPoint(const cbrush_t* brush, const float(*axialPlanes)[4], const __int16* sideIndices, const float* xyz, int pt_count, std::vector<ShowCollisionBrushPt>& brushPoints)
{
	int sIndex = 0;
	do
	{
		if(DotProduct(axialPlanes[sIndex], xyz) - axialPlanes[sIndex][3] > 0.1f)
			return pt_count;


	} while (++sIndex < 6);


	for (int i = 0; i < brush->numsides; i++) {

		const cplane_s* plane = brush->sides[i].plane;

		if (plane != brush->sides[sideIndices[0] - 6].plane
			&& plane != brush->sides[sideIndices[1] - 6].plane
			&& plane != brush->sides[sideIndices[2] - 6].plane
			&& (DotProduct(plane->normal, xyz) - plane->dist) > 0.1f)
			return pt_count;
		

	}

	if (pt_count >= 1024) {
		Com_Printf(CON_CHANNEL_OBITUARY, "More than %i points from plane intersections on %i-sided brush\n", pt_count, brush->numsides);
		return pt_count;
	}

	brushPoints.push_back(std::move(ShowCollisionBrushPt{ .xyz = {xyz[0], xyz[1], xyz[2]},.sideIndex = {sideIndices[0], sideIndices[1], sideIndices[2]}}));
	//VectorCopy(xyz, brushPoints[pt_count].xyz);
	//VectorCopy(sideIndices, brushPoints[pt_count].sideIndex);


	return pt_count+1;
}
void CM_AddExteriorPointToWindingProjected(winding_t* w, const float* pt, int i, int j)
{
	const auto cross_p = [](const float* pt0, const float* pt1, const float* pt2, const int i, const int j) -> float
	{
		return (pt2[j] - pt1[j]) * pt0[i] + (pt0[j] - pt2[j]) * pt1[i] + (pt1[j] - pt0[j]) * pt2[i];
	};

	int bestIndex = -1;
	float best = FLT_MAX;
	int prev = w->numpoints - 1;

	for (int o = 0; o < w->numpoints; o++) {
		
		const float a = cross_p(w->p[prev], pt, w->p[o], i, j);

		if (best > a) {
			best = a;
			bestIndex = o;
		}

		prev = o;
	}

	if (bestIndex < 0) {
		//Com_Printf(CON_CHANNEL_OBITUARY, "^1epic index failure!\n");
		return;
	}

	//std::cout << "best: " << best << '\n';

	if (best < -0.001f) {
		char* offs = (char*)w + 12 * bestIndex;

		memmove((char*)w->p[bestIndex + 1], (char*)w->p[bestIndex], 12 * (w->numpoints - bestIndex));
		VectorCopy(pt, w->p[bestIndex]);

		++w->numpoints;
	}

	else if (best <= 0.001f) {
		//Com_Printf(CON_CHANNEL_OBITUARY, "CM_AddColinearExteriorPointToWindingProjected!\n");
		CM_AddColinearExteriorPointToWindingProjected(w, pt, i, j, (bestIndex + w->numpoints-1) % w->numpoints,bestIndex);
	}

	//std::cout << "numpoints: " << w->numpoints << '\n';

}
float CM_RepresentativeTriangleFromWinding(const winding_t* w, const float* normal, int* i0, int* i1, int* i2)
{
	const auto Vec3Cross = [](const float* v0, const float* v1, float* cross) {
		*cross = (v1[2] * v0[1]) - (v1[1] * v0[2]);
		cross[1] = (v0[2] * *v1) - (v1[2] * *v0);
		cross[2] = (v1[1] * *v0) - (v0[1] * *v1);
	};

	float v0 = 0.0f;
	vec3_t va, vb, vc;

	*i0 = 0;
	*i1 = 1;
	*i2 = 2;
	
	if (w->numpoints <= 2) {
		return v0;
	}

	for (int i = 2; i < w->numpoints; i++) {

		for (int j = 1; j < i; j++) {

			VectorSubtract(w->p[i], w->p[j], vb);

			for (int a = 0; a < j; a++) {

				VectorSubtract(w->p[a], w->p[j], va);
				Vec3Cross(va, vb, vc);

				const float v1 = fabs(DotProduct(vc, normal));
				
				if (v1 > v0) {
					//std::cout << std::format("v1 ({}): [{}, {}, {}]\n", v1, a, j, i);


					v0 = v1;
					*i0 = a;
					*i1 = j;
					*i2 = i;
				}


			}
		}
	}

	return v0;

}
bool PlaneFromPoints(vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c)
{
	vec3_t d1, d2;

	VectorSubtract(b, a, d1);
	VectorSubtract(c, a, d2);


	CrossProduct(d2, d1, plane);

	float len = VectorLength(plane);

	if (!len)
		return false;

	if (len < 2.f) {
		if (VectorLength(d1) * VectorLength(d2) * 0.0000010000001f >= len) {
			VectorSubtract(c, b, d1);
			VectorSubtract(a, b, d2);

			CrossProduct(d2, d1, plane);
			
			if (VectorLength(d1) * VectorLength(d2) * 0.0000010000001f >= len) {
				return false;
			}

		}
	}

	if (VectorNormalize(plane) == 0) {
		return 0;
	}

	plane[3] = DotProduct(a, plane);
	return 1;
}
void CM_ReverseWinding(winding_t* w)
{

	for (int i = 0; i < w->numpoints; i++)
	{

		const float temp[3] =
		{
			w->p[i][0], w->p[i][1], w->p[i][2]
		};

		VectorCopy(w->p[w->numpoints - 1 - i], w->p[i]);
		VectorCopy(temp, w->p[w->numpoints - 1 - i]);


	}

	//for (auto i = 0; i < w->numpoints / 2; ++i)
	//{
	//	const float s_winding[3] =
	//	{
	//		w->p[i][0], w->p[i][1], w->p[i][2]
	//	};

	//	w->p[i][0] = w->p[w->numpoints - 1 - i][0];
	//	w->p[i][1] = w->p[w->numpoints - 1 - i][1];
	//	w->p[i][2] = w->p[w->numpoints - 1 - i][2];

	//	w->p[w->numpoints - 1 - i][0] = s_winding[0];
	//	w->p[w->numpoints - 1 - i][1] = s_winding[1];
	//	w->p[w->numpoints - 1 - i][2] = s_winding[2];
	//}
}
void CM_AddColinearExteriorPointToWindingProjected(winding_t* w, const float* pt, int i, int j, int index0, int index1)
{
	//unsure about this but it's 1:1 to the IDA pseudocode so :shrug:

	//if (w->p[index0][i] == w->p[index1][i]) {
	//	if (w->p[index0][j] == w->p[index1][j]) {
	//		Com_Printf(CON_CHANNEL_OBITUARY, "w->p[index0][i] != w->p[index1][i] || w->p[index0][j] != w->p[index1][j]\n");

	//	}
	//}
	int axis = j;
	float delta = w->p[index1][j] - w->p[index0][j];

	if ((fabs(w->p[index1][i] - w->p[index0][i]) < fabs(w->p[index1][j] - w->p[index0][j])) == false) {
		axis = i;
		delta = w->p[index1][i] - w->p[index0][i];
	}

	if (delta > 0.f) {
		//if (w->p[index1][axis] <= w->p[index0][axis]) {
		//	Com_Printf(CON_CHANNEL_OBITUARY, "w->p[index0][axis] > w->p[index1][axis]\n");
		//}

		if (w->p[index0][axis] <= pt[axis]){
			if (pt[axis] > w->p[index1][axis]){
				VectorCopy(pt, w->p[index1]);
				return;
			}
			VectorCopy(pt, w->p[index1]);
			Com_Printf(CON_CHANNEL_OBITUARY, "ouch!\n");
		}
		else {
			VectorCopy(pt, w->p[index0]);
		}

		return;
	}

	//if (w->p[index0][axis] <= w->p[index1][axis]) {
	//	Com_Printf(CON_CHANNEL_OBITUARY, "w->p[index0][axis] < w->p[index1][axis]\n");
	//}

	

	if (pt[axis] <= w->p[index0][axis]){
		if (w->p[index1][axis] > pt[axis]){
			VectorCopy(pt, w->p[index1]);
			return;
		}
		VectorCopy(pt, w->p[index1]);

	//	Com_Printf(CON_CHANNEL_OBITUARY, "ouch!\n");

	}
	else{
		VectorCopy(pt, w->p[index0]);
		return;
	}

	//VectorCopy(pt, w->p[index1]);


}
bool CM_BuildBrushWindingForSide(cbrush_t* brush, winding_t* winding, float* planeNormal, int sideIndex, const std::vector<ShowCollisionBrushPt>& pts, int ptCount)
{
	float points[1024][3]{};

	int numWindings = CM_GetXyzList(sideIndex, pts, ptCount, points, ptCount);
	int i1, i2, i3;
	////std::cout << "numWindings: " << numWindings << '\n';

	if (numWindings < 3)
		return 0;

	int i = 0, j = 0;

	CM_PickProjectionAxes(planeNormal, &i, &j);

	VectorCopy(points[0], winding->p[0]);
	VectorCopy(points[1], winding->p[1]);
	winding->numpoints = 2;

	//int windings = numWindings - 2;
	//int idx = 0;
	//do
	//{
	//	CM_AddExteriorPointToWindingProjected(winding, points[windings], i, j);

	//	--windings;
	//	idx++;
	//} while (windings);

	for (int a = 2; a < numWindings; a++) {
		CM_AddExteriorPointToWindingProjected(winding, points[a], i, j);
	}

	if (CM_RepresentativeTriangleFromWinding(winding, planeNormal, &i1, &i2, &i3) < 0.001f) {
		return 0;
	}

	vec4_t plane;

	PlaneFromPoints(plane, winding->p[i1], winding->p[i2], winding->p[i3]);

	if (DotProduct(plane, planeNormal) > 0.f) {
		CM_ReverseWinding(winding);
	}

	winding2_t w;

	w.hasBounce = (planeNormal[2] >= 0.3f && planeNormal[2] <= 0.7f);
	w.numpoints = winding->numpoints;
	w.brush = brush;

	for(int i = 0; i < 8; i++)
		for (int j = 0; j < 3; j++)
			w.p[i][j] = winding->p[i][j];

	brushWindings.push_back(std::move(w));

	//if (planeNormal[2] >= 0.3f && planeNormal[2] <= 0.7f)
	//	Com_Printf(CON_CHANNEL_OBITUARY, "^2bounce!\n");

	//for (auto i = 0; i < winding->numpoints; i++) {
		//brushPoints_l.push_back({ winding->p[0][i1], winding->p[0][i2] , winding->p[0][i3] });

		//Com_Printf(CON_CHANNEL_OBITUARY, "adding (%i) numpoints!\n", winding->numpoints);

	//}
	return 1;
}
bool VecNCompareCustomEpsilon(const float* v0, const float* v1, float epsilon, int coordCount)
{
	for (int i = 0; i < coordCount; ++i)
	{
		if (((v0[i] - v1[i]) * (v0[i] - v1[i])) > (epsilon * epsilon))
		{
			return false;
		}
	}

	return true;
}

int CM_GetXyzList(int sideIndex, const std::vector<ShowCollisionBrushPt>& pts, int ptCount, float(*xyz)[3], int xyzLimit)
{

	const auto CM_PointInList = [](const float* point, float(*xyzList)[3], int xyzCount) -> bool {

		int points = 0;
		if (xyzCount <= 0)
			return 0;
		while (!VecNCompareCustomEpsilon(xyzList[points], point, 0.1, 3))
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

			////std::cout << "xyz[" << windingPoints << "][0]: " << xyz[windingPoints][0] << '\n';
			////std::cout << "xyz[" << windingPoints << "][1]: " << xyz[windingPoints][1] << '\n';
			////std::cout << "xyz[" << windingPoints << "][2]: " << xyz[windingPoints][2] << '\n';

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
	if (!brush)
		return;

	float axialPlanes[24];
	std::vector<ShowCollisionBrushPt> pts;
	int numColPoints = 0;

	CM_BuildAxialPlanes((float(*)[6][4])axialPlanes, brush);

	numColPoints = CM_ForEachBrushPlaneIntersection(brush, (float(*)[4])axialPlanes, pts);

	if (numColPoints < 4) {
		return;
	}

	Com_Printf(CON_CHANNEL_OBITUARY, "collision points: ^2%i\n", numColPoints);

	//axial faces
	int side = 0;
	do {
		if (CM_BuildBrushWindingForSide(brush, winding_pool, (axialPlanes + side * 4), side, pts, numColPoints)) {
			
		}
	} while (++side < 6);
	

	//for (auto i = 0; i < numColPoints; i++)
	//	Com_Printf(CON_CHANNEL_OBITUARY, std::format("[^2{}^7] = [^2{}, {}, {}^7]\n", i, pts[i].xyz[0], pts[i].xyz[1], pts[i].xyz[2]).c_str());

	side = 6;

	if ((brush->numsides + 6 > 6) == false)
		return;
	int curSide = 0;
	do {

		CM_BuildBrushWindingForSide(brush, winding_pool, brush->sides[curSide].plane->normal, side, pts, numColPoints);

		++side;
		++curSide;
	} while (side < brush->numsides + 6);
}

void RB_RenderWinding(const winding2_t* w)
{
	std::vector<fvec3> tris;

	int points = std::clamp(w->numpoints, 0, 8);

	const auto only_bounces = find_evar<bool>("Only Bounces");
	const auto drawdist = find_evar<float>("Distance");


	if (w->hasBounce == false && only_bounces->get() || Distance(clients->cgameOrigin, w->brush->get_origin()) > drawdist->get())
		return;

	bool show_all = GetAsyncKeyState(VK_NUMPAD6) & 1;

	RB_DrawCollisionPoly(std::clamp(w->numpoints, 0, 8), (float(*)[3])w->p, vec4_t {0,1,1,0.3f});
	
	//GfxPointVertex verts[4]{};
	//const auto depthtest = find_evar<bool>("Depth Test");

	//for (int i = 0; i < points-1; i++) {


	//	RB_AddDebugLine(verts, depthtest->get(), w->p[i], (float*)w->p[i + 1], vec4_t{1.f,0.f,0.f,1.f}, 0);
	//	RB_DrawLines3D(1, 3, verts, depthtest->get());

	//}

}

void CM_FindRandomBrushByName()
{
	static bool show_all_triggers = false;

	if (cmd_args->argc[cmd_args->nesting] != 2) {
		if (!show_all_triggers)
			Com_Printf(CON_CHANNEL_CONSOLEONLY, "usage: showbrush <material>");
		show_all_triggers = false;

		return;
	}
	auto name = *(cmd_args->argv[cmd_args->nesting] + 1);
	show_all_triggers = true;


	int idx = int(random(cm->numBrushes));
	while (!&cm->brushes[idx] ||
		std::string(cm->materials[cm->brushes[idx].axialMaterialNum[0][0]].material).find(name) == std::string::npos) {
		idx = int(random(cm->numBrushes));
	}

	brushWindings.clear();
	


	for (int i = 0; i < cm->numBrushes; i++) {

		if (std::string(cm->materials[cm->brushes[i].axialMaterialNum[0][0]].material).find(name) == std::string::npos)
			continue;
			

		CM_GetPolys(&cm->brushes[i]);
	}
	

	//VectorCopy(cm->brushes[idx].maxs, ps_loc->origin);
}
bool CM_BrushInView(const cbrush_t* brush, struct cplane_s* frustumPlanes, int numPlanes)
{
	if (numPlanes <= 0)
		return 1;

	cplane_s* plane = frustumPlanes;
	int idx = 0;
	while ((BoxOnPlaneSide(brush->mins, brush->maxs, plane) & 1) != 0) {
		++plane;
		++idx;

		if (idx >= numPlanes)
			return 1;
	}

	return 0;
}
void RB_ShowCollision(GfxViewParms* viewParms)
{
	if (brushWindings.empty() && cm_terrainpoints.empty())
		return;

	cplane_s frustum_planes[6];

	BuildFrustumPlanes(viewParms, frustum_planes);

	frustum_planes[5].normal[0] = -frustum_planes[4].normal[0];
	frustum_planes[5].normal[1] = -frustum_planes[4].normal[1];
	frustum_planes[5].normal[2] = -frustum_planes[4].normal[2];

	frustum_planes[5].dist = -frustum_planes[4].dist - 2000;
	auto plane = &frustum_planes[5];

	char signbit = 0;

	if (plane->normal[0] != 1.f) {
		if (plane->normal[1] == 1.f)
			signbit = 1;
		else {
			signbit = plane->normal[2] == 1.f ? 2 : 3;
		}
	}

	plane->type = signbit;

	SetPlaneSignbits(plane);

	for (auto& i : brushWindings)
		if (CM_BrushInView(i.brush, frustum_planes, 5))
			RB_RenderWinding(&i);

	for (auto& i : cm_terrainpoints) {
		CM_ShowTerrain(&i, frustum_planes);
	}


}
void RB_DrawCollisionPoly(int numPoints, float(*points)[3], const float* colorFloat)
{
	uint8_t c[4];
	std::vector<fvec3> pts;
	const auto depthtest = find_evar<bool>("Depth Test");

	R_ConvertColorToBytes(colorFloat, c);
	
	for (int i = 0; i < numPoints; i++)
		pts.push_back(points[i]);


	//GfxPointVertex verts[4]{};

	//for (int i = 0; i < numPoints -1; i++) {


	//	RB_AddDebugLine(verts, depthtest->get(), points[i], (float*)points[i + 1], c, 0);
	//	RB_DrawLines3D(1, 3, verts, depthtest->get());

	//}

	RB_DrawPolyInteriors(numPoints, pts, c, true, depthtest->get());

}