#pragma once

#include "pch.hpp"

typedef long(__stdcall* endScene)(IDirect3DDevice9*);

class Renderer
{
public:
	static Renderer& getInstance()
	{
		static Renderer r;
		return r;
	}

	bool initialize();

	static long __stdcall EndSceneRenderer(IDirect3DDevice9* device);
	static void __cdecl CG_DrawActive();
	endScene endscene_getter() const noexcept { return pEndScene; }

	static void CL_ShutdownRenderer();
	static void R_RecoverLostDevice();
private:


	std::optional<endScene> get_endscene();

	void initialize_imgui();

	bool begin_frame();
	void end_frame();

	HWND h_hWnd = 0;
	endScene pEndScene = 0;
	IDirect3DDevice9* pDevice = nullptr;
};