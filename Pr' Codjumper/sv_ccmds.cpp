#include "pch.hpp"
void CL_Disconnect()
{
	static decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CL_DISCONNECT);

	if (clientUI[0].connectionState != CA_DISCONNECTED) { //gets called in the loading screen in 1.7

		Com_Printf(CON_CHANNEL_CONSOLEONLY, "removing hooks because player disconnected\n");
		Cmd_RemoveCommand("1_kej_openmenu");
		CG_ReleaseHooks();



	}

	detour_func.cast_call<void(*)()>();
}
void SV_Map(void* a1)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_SV_MAP);


	const dvar_s* fs_game = Dvar_FindMalleableVar("fs_game");
	const dvar_s* g_gametype = Dvar_FindMalleableVar("g_gametype");
	const dvar_s* sv_running = Dvar_FindMalleableVar("sv_running");

	std::cout << "hi!\n";
	detour_func.cast_call<void(*)(void*)>(a1);

	std::cout << "hi again!\n";

	//if (fs_game && g_gametype && sv_running) {

		if (sv_cmd_args->argc[sv_cmd_args->nesting] < 1)
			return;
	
		//if (!sv_running->current.enabled) {
		//	Com_PrintError(CON_CHANNEL_CONSOLEONLY, "failed to [%s] because '%s' is missing\n", *(sv_cmd_args->argv[sv_cmd_args->nesting] + 0), *(sv_cmd_args->argv[sv_cmd_args->nesting] + 1));
		//	return;
		//}

		dvar_s* sv_punkbuster = Dvar_FindMalleableVar("sv_punkbuster");
		dvar_s* cl_punkbuster = Dvar_FindMalleableVar("cl_punkbuster");

		if (sv_punkbuster) { sv_punkbuster->current.integer = 0; sv_punkbuster->current.enabled = 0; }
		if (cl_punkbuster) { cl_punkbuster->current.integer = 0; cl_punkbuster->current.enabled = 0; }

		CG_CreateHooks();


	//}
	return;
}