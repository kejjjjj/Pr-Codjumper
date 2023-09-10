#include "pch.hpp"

void CL_FixServerTime(usercmd_s* cmd)
{
	static dvar_s* com_maxfps = Dvar_FindMalleableVar("com_maxfps");
	static int first_time = cmd->serverTime;
	static int stime = 0;

	if (abs(cmd->serverTime - first_time) > 100)
		first_time = cmd->serverTime;
	cmd->serverTime = first_time;
	clients->serverTime = cmd->serverTime;
	first_time += 1000 / com_maxfps->current.integer == 0 ? 1 : com_maxfps->current.integer;
}
void CL_FinishMove(usercmd_s* cmd)
{

	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CL_FINISHMOVE);
	detour_func.cast_call<void(*)(usercmd_s*)>(cmd);

	static Playback& playback = Playback::getInstance();
	static dvar_s* com_maxfps = Dvar_FindMalleableVar("com_maxfps");
	static bool playbackDelay = false;
	static DWORD ms = Sys_MilliSeconds();


	//CL_FixServerTime(cmd);

	if (GetAsyncKeyState(VK_NUMPAD4) & 1) {
		if (!playback.isRecording()) {
			playback.clear();
			Com_Printf(CON_CHANNEL_OBITUARY, "record!\n");
			playback.StartRecording(cmd->angles);
		}
		else {
			Com_Printf(CON_CHANNEL_OBITUARY, "stop!\n");
			playback.StopRecording();
		}
	}
	
	if (playback.isRecording()) {
		playback.Record(cmd, com_maxfps->current.integer);
	}
	else if (GetAsyncKeyState(VK_NUMPAD6) & 1) {
		if (!playback.IsPlayback() && !playbackDelay) {
			


			ms = Sys_MilliSeconds();
			playback.StartPlayback();
			playbackDelay = true;

			cgs->predictedPlayerState = playback.state;
			*ps_loc = playback.state;
			auto& it = playback.getIterator().get();
			
			cmd->angles[0] = it->angles[0];
			cmd->angles[1] = it->angles[1];
			cmd->angles[2] = it->angles[2];


			//CG_SetPlayerAngles(playback.getIterator().get()->viewangles);

		}
	}

	//wait 1 second before playback
	if (playbackDelay) {

		if (Sys_MilliSeconds() > ms + 1000) {
			playbackDelay = false;
			cgs->predictedPlayerState = playback.state;
			*ps_loc = playback.state;
		}

	}
	else if (playback.IsPlayback()) {

		if ((GetAsyncKeyState('W') < 0) || (GetAsyncKeyState('A') < 0) || (GetAsyncKeyState('S') < 0) || (GetAsyncKeyState('D') < 0))
			playback.StopPlayback();
		else
			playback.doPlayback(cmd);
	}


	return;

}