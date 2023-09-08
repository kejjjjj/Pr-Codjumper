#pragma once

#include "pch.hpp"

struct pm_controller_t
{

	char forwardmove;
	char rightmove;
	int buttons;
	int FPS;
	int numLoops;

	std::optional<fvec3> velocity;
	std::optional<fvec3> origin;
	std::optional<fvec3> viewangles;
	fvec3 turnrate;

	bool strafebot = false;
};
class PM_MovementSimulation
{
public:
	PM_MovementSimulation(pmove_t* pm, pml_t* pml, pm_controller_t&& controls);
	PM_MovementSimulation(pmove_t* pm, pml_t* pml);

	void SetController(pm_controller_t&& controls) { controller = std::make_unique<pm_controller_t>(std::move(controls)); }

	void StartSimulation();

	pmove_t* GetPM() noexcept { return &pm_og; }
	pml_t* GetPML() noexcept { return &pml_og; }
	pm_controller_t* GetController() noexcept { return controller.get(); }
	PM_MovementSimulation() = delete;
	PM_MovementSimulation(PM_MovementSimulation& other) = delete;
	PM_MovementSimulation operator=(PM_MovementSimulation& other) = delete;
private:

	void PmoveSingle(pmove_t* pm, pml_t* pml);

	pmove_t pm_og;
	pml_t pml_og;
	playerState_s ps;
	std::unique_ptr<pm_controller_t> controller;
	
	void(__cdecl* PM_CheckLadderMove_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x413A60);
	void(__cdecl* PM_LadderMove_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x413DD0);
	void(__cdecl* PM_AdjustAimSpreadScale_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x418870);
	void(*PM_UpdateSprint_)(pmove_t* pm) = ((void(__cdecl*)(pmove_t*))0x40E6A0);
	void(*PM_UpdateAimDownSightFlag_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x416E60);
	//void(*PM_Footsteps_)(pmove_t* pm, pml_t* pml) = ((void(__cdecl*)(pmove_t*, pml_t*))0x412180);
	void PM_OverBounce(pmove_t* pm, pml_t* pml);

	void PM_Footsteps_(pmove_t* pm, pml_t* pml)
	{
		__asm
		{
			push pml;
			mov eax, pm;
			mov esi, 0x412180;
			call esi;
			add esp, 0x4;
		}
	}
	void PM_GroundTrace_(pmove_t* pm, pml_t* pml)
	{
		__asm
		{
			push pml;
			push pm;
			mov esi, 0x00410660;
			call esi;
			add esp, 0x8;
		}
	}
	static void PM_DropTimers(playerState_s* ps, pml_t* pml)
	{
		uintptr_t fnc = 0x00412510;
		__asm
		{
			mov eax, ps;
			mov esi, pml;
			call fnc;
		}
	}
	static void PM_UpdatePronePitch(pml_t* pml, pmove_t* pm)
	{
		__asm
		{
			push pm;
			mov edi, pml;
			mov esi, 0x4136E0;
			call esi;
			add esp, 0x4;
		}
	}
	static void PM_CheckDuck_(pmove_t* pm, pml_t* pml)
	{
		__asm
		{
			lea ecx, pml;
			push ecx;
			mov edi, pm;
			mov esi, 0x410F70;
			call esi;
			add esp, 0x4;
		}
	}
	static void PM_UpdatePlayerWalkingFlag_(pmove_t* pm) {
		__asm
		{
			mov ecx, pm;
			mov esi, 0x413A10;
			call esi;

		}
	}
	static void PM_Weapon(pmove_t* pm, pml_t* pml)
	{
		__asm
		{
			mov eax, pml;
			mov ecx, pm;
			mov esi, 0x0041A470;
			call esi;
		}
	}
	static void PM_UpdateViewAngles(playerState_s* ps, float msec, usercmd_s* cmd, char handler) {

		__asm
		{

			movzx eax, handler;
			mov esi, cmd;
			push eax;
			push esi;
			push msec;
			mov eax, ps;
			mov esi, 0x413580;
			call esi;
			add esp, 0xC;
		}

	}

};
 