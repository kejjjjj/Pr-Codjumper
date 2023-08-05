#include "pch.hpp"


void PlayerCmd_GetButtonPressed(scr_entref_t arg)
{

	int32_t ent(0);
	gentity_s* gent;
	if (HIWORD(arg)) {
		Scr_ObjectError("Not an entity");
		gent = NULL;
	}
	else {
		ent = LOWORD(arg);
		gent = Scr_GetEntity(arg);
		if (!gent->client) {
			return Scr_ObjectError("Entity: [%i] is not a player", ent);
		}
	}
	if (Scr_GetNumParam() != 0)
		return Scr_ObjectError("Usage: player GetButtonPressed()");

	const usercmd_s* cmd = cinput->GetUserCmd(cinput->currentCmdNum - 1);

	if (!cmd) {
		Scr_AddInt(0);
		return;
	}
	Scr_AddInt(cmd->buttons);
}
void PlayerCmd_GetForwardMove(scr_entref_t arg)
{

	int32_t ent(0);
	gentity_s* gent;
	if (HIWORD(arg)) {
		Scr_ObjectError("Not an entity");
		gent = NULL;
		Scr_AddInt(0);
		return;
	}
	else {
		ent = LOWORD(arg);
		gent = Scr_GetEntity(arg);
		if (!gent->client) {
			Scr_ObjectError("Entity: [%i] is not a player", ent);
			Scr_AddInt(0);
			return;
		}
	}
	if (Scr_GetNumParam() > 0) {
		Scr_ObjectError("Usage: player GetForwardMove()");
		Scr_AddInt(0);
		return;
	}
	const usercmd_s* cmd = cinput->GetUserCmd(cinput->currentCmdNum - 1);

	if (!cmd) {
		Scr_AddInt(0);
		return;
	}

	Scr_AddInt(cmd->forwardmove);
}
void PlayerCmd_GetRightMove(scr_entref_t arg)
{

	int32_t ent(0);
	gentity_s* gent;
	if (HIWORD(arg)) {
		Scr_ObjectError("Not an entity");
		gent = NULL;
		Scr_AddInt(0);
		return;
	}
	else {
		ent = LOWORD(arg);
		gent = Scr_GetEntity(arg);
		if (!gent->client) {
			Scr_ObjectError("Entity: [%i] is not a player", ent);
			Scr_AddInt(0);
			return;
		}
	}
	if (Scr_GetNumParam() > 0) {
		Scr_ObjectError("Usage: player GetRightMove()");
		Scr_AddInt(0);
		return;
	}
	const usercmd_s* cmd = cinput->GetUserCmd(cinput->currentCmdNum - 1);

	if (!cmd) {
		Scr_AddInt(0);
		return;
	}

	Scr_AddInt(cmd->rightmove);
}
void PlayerCmd_SetVelocity(scr_entref_t arg)
{

	int32_t ent(0);
	gentity_s* gent;
	vec3_t velocity{};
	if (HIWORD(arg)) {
		Scr_ObjectError("Not an entity");
		gent = NULL;
		return;
	}
	else {
		ent = LOWORD(arg);
		gent = Scr_GetEntity(arg);
		if (!gent->client) {
			Scr_ObjectError("Entity: [%i] is not a player", ent);
			return;
		}
	}
	if (Scr_GetNumParam() != 1) {
		Scr_ObjectError("Usage: player setVelocity( vec3 )");
		return;
	}
	Scr_GetVector(0, velocity);
	VectorCopy(velocity, gent->client->ps.velocity);
}
void GScr_SendCommand()
{
	if (Scr_GetNumParam() != 1) {
		Scr_ObjectError("Usage: SendCommand( string )");
		return;
	}
	char* str = Scr_GetString(0);

	if (str) {
		CBuf_Addtext(std::string(std::string(str) + '\n').c_str());
	}
}