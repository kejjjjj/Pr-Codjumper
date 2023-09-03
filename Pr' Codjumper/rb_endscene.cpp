#include "pch.hpp"


void RB_DrawPolyInteriors(int n_points, std::vector<fvec3>& points, const BYTE* color, bool two_sided, bool depthTest, bool show)
{
	//partly copied from iw3xo :)
	if (n_points < 3)
		return;


	bool show_all = GetAsyncKeyState(VK_NUMPAD6) & 1;
	if (show) {
		std::cout << "tris[0]: {" << points[0].x << ", " << points[0].y << ", " << points[0].z << "}\n";
		std::cout << "tris[1]: {" << points[1].x << ", " << points[1].y << ", " << points[1].z << "}\n";
		std::cout << "tris[2]: {" << points[2].x << ", " << points[2].y << ", " << points[2].z << "}\n";

	}

	static Material whiteMaterial = *rgp->whiteMaterial;
	static uint32_t ogBits = whiteMaterial.stateBitsTable->loadBits[1];

	Material* material = &whiteMaterial;

	if (gfxCmdBufState->origMaterial != material || gfxCmdBufState->origTechType != MaterialTechniqueType::TECHNIQUE_UNLIT) {
		if (tess->indexCount)
			RB_EndTessSurface();

		if (depthTest)
			material->stateBitsTable->loadBits[1] = 44;
		else
			material->stateBitsTable->loadBits[1] = ogBits;

		if (two_sided)
			material->stateBitsTable->loadBits[0] = 422072677;
		else
			material->stateBitsTable->loadBits[0] = 422089061;

		RB_BeginSurface(MaterialTechniqueType::TECHNIQUE_UNLIT, material);

	}
	if (n_points + tess->vertexCount > 5450 || tess->indexCount + 2 * (n_points - 2) > 1048576)// RB_CheckTessOverflow
	{
		RB_EndTessSurface();
		RB_BeginSurface(gfxCmdBufState->origTechType, gfxCmdBufState->origMaterial);
	}
	int idx = 0;

	for (; idx < n_points; ++idx) {
		vec3_t p = { points[idx].x, points[idx].y, points[idx].z };
		RB_SetPolyVertice(p, color, tess->vertexCount + idx, idx);
	}

	for (idx = 2; idx < n_points; ++idx)
	{
		tess->indices[tess->indexCount + 0] = tess->vertexCount;
		tess->indices[tess->indexCount + 1] = (idx + tess->vertexCount);
		tess->indices[tess->indexCount + 2] = (idx + tess->vertexCount - 1);
		tess->indexCount += 3;
	}

	tess->vertexCount += n_points;

	RB_EndTessSurface();

}
int RB_AddDebugLine(GfxPointVertex* verts, char depthTest, const vec_t* start, vec_t* end, const vec_t* color, int vertCount)
{
	int _vc = vertCount;
	uint8_t _color[4]{ 0,0,0,0 };
	if (vertCount + 2 > 2725)
	{
		RB_DrawLines3D(vertCount / 2, 1, verts, depthTest);
		_vc = 0;
	}

	GfxPointVertex* vert = &verts[_vc];
	if (color) {
		R_ConvertColorToBytes(color, vert->color);
	}

	verts[_vc + 1].color[0] = vert->color[0];
	verts[_vc + 1].color[1] = vert->color[1];
	verts[_vc + 1].color[2] = vert->color[2];
	verts[_vc + 1].color[3] = vert->color[3];

	VectorCopy(start, vert->xyz);

	vert = &verts[_vc + 1];
	VectorCopy(end, vert->xyz);


	return _vc + 2;

	//return ((int(*)(GfxPointVertex *, char, const vec_t *, vec_t *, const vec_t *, int))0x658210)(verts, depthTest, start, end, color, vertCount);
}
void R_ConvertColorToBytes(const vec4_t in, uint8_t* out)
{
	//__asm
	//{
	//	lea edx, out;
	//	mov ecx, in;
	//	mov esi, 0x493530;
	//	call esi;
	//}
	((char(__fastcall*)(const float* in, uint8_t * out))0x493530)(in, out);

	return;
}
char RB_DrawLines3D(int count, int width, GfxPointVertex* verts, char depthTest)
{
	((char(__cdecl*)(int, int, GfxPointVertex*, char))0x613040)(count, width, verts, depthTest);
	return 1;

}
int RB_BeginSurface(MaterialTechniqueType tech, Material* material)
{
	int rval = 0;
	const static DWORD fnc = 0x61A220;
	__asm
	{
		mov edi, tech;
		mov esi, material;
		call fnc;
		mov rval, eax;
	}
	return rval;
}
void RB_EndTessSurface()
{
	((void(*)())0x61A2F0)();

}
void RB_SetPolyVertice(const vec3_t pos, const BYTE* col, const int vert, const int index)
{
	VectorCopy(pos, tess->verts[vert].xyzw);
	//tess->verts[vert].color.packed = 0xFF00FFAA;
	tess->verts[vert].color.array[0] = col[0];
	tess->verts[vert].color.array[1] = col[1];
	tess->verts[vert].color.array[2] = col[2];
	tess->verts[vert].color.array[3] = col[3];

	//std::cout << "color: " << std::hex << tess->verts[vert].color.packed << '\n';

	switch (index)
	{
	case 0:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;

	case 1:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 1.0f;
		break;
	case 2:
		tess->verts[vert].texCoord[0] = 1.0f;
		tess->verts[vert].texCoord[1] = 1.0f;
		break;
	case 3:
		tess->verts[vert].texCoord[0] = 1.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;

	default:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;
	}

	tess->verts[vert].normal.packed = 1073643391;
}
char RB_DrawDebug(GfxViewParms* viewParms)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_RB_ENDSCENE);


	for (auto& i : brushWindings)
		RB_RenderWinding(&i);

	return detour_func.cast_call<char(*)(GfxViewParms*)>(viewParms);
}

void RB_DrawTriangleOutline(vec3_t points[3], vec4_t color, int width, bool depthTest)
{
	GfxPointVertex verts[6]{};



	RB_AddDebugLine(verts, depthTest, points[0], points[1], color, 0);
	RB_AddDebugLine(verts, depthTest, points[0], points[2], color, 2);
	RB_AddDebugLine(verts, depthTest, points[1], points[2], color, 4);

	RB_DrawLines3D(3, width, verts, depthTest);

}