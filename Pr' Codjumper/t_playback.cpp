#include "pch.hpp"

//
//void Playback::Record(usercmd_s* cmd, const int FPS) noexcept
//{
//	if (recording == false)
//		return;
//
//	recorder_cmd rcmd;
//
//	VectorCopy(cmd->angles, rcmd.angles);
//	//VectorSubtract(cmd->angles, recordingAngle, rcmd.angles);
//	rcmd.buttons = cmd->buttons;
//	rcmd.forwardmove = cmd->forwardmove;
//	rcmd.rightmove = cmd->rightmove;
//	rcmd.FPS = FPS;
//	rcmd.offhand = cmd->offHandIndex;
//	rcmd.origin.x = clients->cgameOrigin[0];
//	rcmd.origin.y = clients->cgameOrigin[1];
//	rcmd.origin.z = clients->cgameOrigin[2];
//	rcmd.serverTime = cmd->serverTime;
//	rcmd.velocity = fabs(clients->cgameVelocity[0] * clients->cgameVelocity[0] + clients->cgameVelocity[1] * clients->cgameVelocity[1]);
//	rcmd.weapon = cmd->weapon;
//
//	VectorCopy(clients->cgameViewangles, rcmd.viewangles);
//
//	recorder_sequence.push_back(std::move(rcmd));
//
//}


void Playback::StartPlayback() noexcept
{
	if (recorder_sequence.empty())
		return;
	//Com_Printf(CON_CHANNEL_SUBTITLE, "Begin playback..\n");

	playback = true;
	it = recorder_sequence.begin();
	frame = 0;
	refresh_start_time = true;

	//VectorSubtract(real_angles, it->angles, angleOffset);
}
void Playback::doPlayback(usercmd_s* cmd) noexcept
{
	static dvar_s* com_maxfps = Dvar_FindMalleableVar("com_maxfps");
	if (refresh_start_time) {
		refTime = cmd->serverTime;
		refresh_start_time = false;
	}

	for (int i = 0; i < 3; i++) {
		cmd->angles[i] = it->angles[i];
	}

	cmd->serverTime = refTime + ((it)->serverTime - recorder_sequence.front().serverTime);
	cmd->weapon = (it)->weapon;
	cmd->offHandIndex = (it)->offhand;
	com_maxfps->current.integer = (it)->FPS;

	clients->serverTime = cmd->serverTime;
	cmd->buttons = (it)->buttons;
	cmd->forwardmove = (it)->forwardmove;
	cmd->rightmove = (it)->rightmove;
	++it;
	++frame;

	if (it == recorder_sequence.end()) {
		playback = false;
		Com_Printf(CON_CHANNEL_OBITUARY, "End playback..\n");
		refresh_start_time = true;
		return;
	}

}
playback_cmd* Playback::CurrentCmd() const {
	if (!playback)
		return 0;

	if (it == recorder_sequence.end() || it == recorder_sequence.begin())
		return nullptr;

	auto copy = it;

	return &(*--copy);
}
void Playback::DrawPlayback()
{
	if (isRecording()) {
		float col[4] = { 0,1,0,1 };
		float glowCol[4] = { 0,0,0,0 };
		R_AddCmdDrawTextWithEffects((char*)"recording", "fonts/normalfont", 940.f, 480.f, 1.3f, 1.3f, 0.f, col, 3, glowCol, nullptr, nullptr, 0, 0, 0, 0);
		return;
	}

	if (!IsPlayback())
		return;

	auto current = CurrentCmd();

	if (!current)
		return;

	float dist = current->origin.dist(clients->cgameOrigin);

	char buff[64];

	sprintf_s(buff, "%.6f\ntime: %d", dist, current->serverTime);
	R_AddCmdDrawTextWithEffects(buff, "fonts/objectivefont", cgs->refdef.width / 1.5f - strlen(buff) * 2, cgs->refdef.height / 1.5f, 1.f, 1.f, 0, vec4_t{ 1,1,1,1 }, 3, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);


}