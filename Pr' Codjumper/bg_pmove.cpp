
#include "pch.hpp"
bool PM_DisallowMouse(const std::optional<bool>& state)
{
	static bool bState = false;

	if (state)
		bState = state.value();

	return bState;
}
void PM_AirMove(pmove_t* pm, pml_t* pml)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_PM_AIRMOVE);

	memcpy(pm_glob, pm, sizeof(pmove_t));
	memcpy(pml_glob, pml, sizeof(pml_t));

	T::Movement::T_Strafebot(pm, pml);

	return detour_func.cast_call<void(__cdecl*)(pmove_t* pm, pml_t* pml)>(pm, pml);
}
void PM_WalkMove(pmove_t* pm, pml_t* pml)
{
	memcpy(pm_glob, pm, sizeof(pmove_t));
	memcpy(pml_glob, pml, sizeof(pml_t));

	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_PM_WALKMOVE);

	T::Movement::T_Strafebot(pm, pml);

	return detour_func.cast_call<void(__cdecl*)(pmove_t * pm, pml_t * pml)>(pm, pml);
}

void __cdecl UpdateViewAngles(float msec, char handler)
{
	usercmd_s* cmd;
	playerState_s* ps;

	__asm retn;
	__asm mov ps, eax;
	__asm mov cmd, esi;
	
	double v4; // st6
	int pm_type; // eax
	double result; // st7
	int pm_flags; // eax
	__int16 v9; // fps
	bool v10; // c0
	char v11; // c2
	bool v12; // c3
	int _pm_type; // eax
	float oldViewYawa; // [esp+20h] [ebp-4h]
	float oldViewYaw; // [esp+20h] [ebp-4h]

	

	pm_type = ps->pm_type;
	if (pm_type != PM_INTERMISSION)
	{
		if (pm_type >= PM_DEAD)
		{
			if (ps->stats[STAT_DEAD_YAW] == 999)
			{
				oldViewYawa = cmd->angles[1] * 0.0054931640625 + ps->delta_angles[1];
				ps->stats[STAT_DEAD_YAW] = (AngleNormalize360(oldViewYawa) * 0.0054931640625);
			}

			result = msec;
			
			auto ah = (&pmoveHandlers)[2 * handler];

			__asm
			{
				mov eax, cmd;
				mov edi, ps;
				push ah;
				fstp[msec];
				push ecx;
				mov esi, 0x412590; //PM_UpdateLean
				call esi;
				add esp, 0x8;

			}

			//PM_UpdateLean(cmd, ps, msec, (&pmoveHandlers)[2 * handler]);
			return;
		}
		result = ps->viewangles[1];
		oldViewYaw = ps->viewangles[1];


		__asm
		{
			mov edi, ps;
			push cmd;
			mov esi, 0x412980; //PM_UpdateViewAngles_Clamp
			call esi;
			add esp, 0x4;
		}

		//if (GetAsyncKeyState(VK_NUMPAD3) & 1 ) {
		//	Com_Printf(CON_CHANNEL_OBITUARY, "yaw: ^1%.2f\n", ps->viewangles[YAW]);
		//	float delta = (ps->viewangles[YAW] -  69);

		//	Com_Printf(CON_CHANNEL_OBITUARY, "angle delta: ^2%.2f\n", delta);

		//	ps->viewangles[YAW] += delta;
		//	cmd->angles[YAW] += ANGLE2SHORT(delta);
		//	ps->delta_angles[YAW] -= (delta);

		//}

	}


	return;

	//return find_hook(hookEnums_e::HOOK_PM_UPDATE_VIEWANGLES).cast_call<void(*)(playerState_s*, float, usercmd_s*, char)>(ps, msec, cmd, handler);

}
void WTF(playerState_s* ps, float msec, usercmd_s* cmd, char handler) 
{


	double v4; // st6
	int pm_type; // eax
	double result; // st7
	int pm_flags; // eax
	__int16 v9; // fps
	bool v10; // c0
	char v11; // c2
	bool v12; // c3
	int _pm_type; // eax
	float oldViewYawa; // [esp+20h] [ebp-4h]
	float oldViewYaw; // [esp+20h] [ebp-4h]

	const bool disallowMouse = PM_DisallowMouse();
	
	float oldYaw = ps->viewangles[YAW];
	float oldCmdYaw = cmd->angles[YAW];
	float oldDeltaYaw = ps->delta_angles[YAW];


	//if (PM_DisallowMouse())
	//	return;

	msec = cgs->frametime;


	pm_type = ps->pm_type;
	if (pm_type != PM_INTERMISSION)
	{
		if (pm_type >= PM_DEAD)
		{
			if (ps->stats[STAT_DEAD_YAW] == 999)
			{
				oldViewYawa = cmd->angles[1] * 0.0054931640625 + ps->delta_angles[1];
				ps->stats[STAT_DEAD_YAW] = (AngleNormalize360(oldViewYawa) * 0.0054931640625);
			}

			result = msec;

			auto ah = (&pmoveHandlers)[2 * handler];

			__asm
			{
				mov eax, cmd;
				mov edi, ps;
				push ah;
				fstp[msec];
				push ecx;
				mov esi, 0x412590; //PM_UpdateLean
				call esi;
				add esp, 0x8;

			}

			//PM_UpdateLean(cmd, ps, msec, (&pmoveHandlers)[2 * handler]);
			return;
		}
		result = ps->viewangles[1];
		oldViewYaw = ps->viewangles[1];


		__asm
		{
			mov edi, ps;
			push cmd;
			mov esi, 0x412980; //PM_UpdateViewAngles_Clamp
			call esi;
			add esp, 0x4;
		}

		//if (GetAsyncKeyState(VK_NUMPAD3) & 1 ) {
		//	Com_Printf(CON_CHANNEL_OBITUARY, "yaw: ^1%.2f\n", ps->viewangles[YAW]);
		//	float delta = (ps->viewangles[YAW] -  69);

		//	Com_Printf(CON_CHANNEL_OBITUARY, "angle delta: ^2%.2f\n", delta);

		//	ps->viewangles[YAW] += delta;
		//	cmd->angles[YAW] += ANGLE2SHORT(delta);
		//	ps->delta_angles[YAW] -= (delta);

		//}

	}


	if (PM_DisallowMouse()) {
		ps->viewangles[YAW] = oldYaw;
		//ps->delta_angles[YAW] = oldDeltaYaw;
		//cmd->angles[YAW] = oldCmdYaw;
		return;
	}


	return;
}
__declspec(naked) void PM_UpdateViewAngles(playerState_s* ps, float msec, usercmd_s* cmd, char handler)
{
	static float yup;
	__asm
	{
		sub esp, 0x8;
		push ebx;
		mov ebx, dword ptr[esp + 0x18]; //handler
		push ebp;
		mov ebp, [esp + 0x18]; //cmd
		push esi;
		push edi;

		mov edi, eax;

		push ebx; //handler
		push ebp;
		push[esp + 0x60-0x8]; //msec
		push edi; //ps
		

		call WTF;
		add esp, 0x10;

		pop edi;
		pop esi;
		pop ebp;
		pop ebx;
		add esp, 8;

		retn;
	}
}
void Sys_SnapVector(float* v)
{
	int i;
	float f;

	f = *v;
	__asm	fld		f;
	__asm	fistp	i;
	*v = i;
	v++;
	f = *v;
	__asm	fld		f;
	__asm	fistp	i;
	*v = i;
	v++;
	f = *v;
	__asm	fld		f;
	__asm	fistp	i;
	*v = i;
	/*
	*v = fastftol(*v);
	v++;
	*v = fastftol(*v);
	v++;
	*v = fastftol(*v);
	*/
}