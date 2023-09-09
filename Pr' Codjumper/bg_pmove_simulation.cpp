#include "pch.hpp"

PM_MovementSimulation::PM_MovementSimulation(pmove_t* pm, pml_t* pml, pm_controller_t&& controls)
{
	memcpy(&pm_og, pm, sizeof(pmove_t));
	memcpy(&pml_og, pml, sizeof(pml_t));
	memcpy(&ps, pm_og.ps, sizeof(playerState_s));
	pm_og.ps = &ps;

	controller = std::make_unique<pm_controller_t>(std::move(controls));
}
PM_MovementSimulation::PM_MovementSimulation(pmove_t* pm, pml_t* pml)
{
	memcpy(&pm_og, pm, sizeof(pmove_t));
	memcpy(&pml_og, pml, sizeof(pml_t));
	memcpy(&ps, pm_og.ps, sizeof(playerState_s));
	pm_og.ps = &ps;
}

void PM_MovementSimulation::StartSimulation()
{
	pmove_t* pm = &pm_og;
	pml_t* pml = &pml_og;

	pm->cmd.buttons = controller->buttons;
	pm->cmd.forwardmove = controller->forwardmove;
	pm->cmd.rightmove = controller->rightmove;

	if (controller->viewangles.has_value()) { VectorCopy((float*)controller->viewangles.value(), pm->ps->viewangles); }
	if (controller->velocity.has_value()) {	VectorCopy((float*)controller->velocity.value(), pm->ps->velocity);}
	if (controller->origin.has_value()) { VectorCopy((float*)controller->origin.value(), pm->ps->origin); }

	for(int i = 0; i < controller->numLoops; i++)
		PmoveSingle(pm, pml);

}

void PM_MovementSimulation::PmoveSingle(pmove_t* pm, pml_t* pml)
{
	playerState_s* ps = pm->ps;
	static dvar_s* mantle_enable = Dvar_FindMalleableVar("mantle_enable");


	if ((pm->ps->pm_flags & PMF_MELEE_CHARGE) != 0)
		pm->cmd.forwardmove = 127;

	if ((ps->pm_flags & PMF_FROZEN) != 0)
	{
		pm->cmd.buttons &= 0x1300u;
	}

	if ((ps->pm_flags & PMF_RESPAWNED) != 0)
	{
		pm->cmd.buttons &= 0x1301u;
	}

	if (ps->weaponstate == WEAPON_FIRING)
	{
		int weaponIdx = ps->weapon;
		if (weaponIdx)
		{
			if (BG_WeaponNames[weaponIdx]->freezeMovementWhenFiring)
			{
				pm->cmd.buttons &= 0xFFFFFB3F;
				pm->cmd.forwardmove = 0;
				pm->cmd.rightmove = 0;
				ps->velocity[0] = 0.0;
				ps->velocity[1] = 0.0;
				ps->velocity[2] = 0.0;
			}
		}
	}

	ps->pm_flags &= 0xFFFFEFFF;
	if (ps->pm_type >= PM_DEAD)
		pm->tracemask &= 0xFDFF3FFF;

	if ((ps->pm_flags & PMF_PRONE) == 0 || BG_WeaponNames[ps->weapon]->overlayReticle && ps->fWeaponPosFrac > 0.0) {
		ps->pm_flags &= 0xFFFFFDFF;
	}

	//if (PM_InteruptWeaponWithProneMove_(ps))
	//{
	//	ps->pm_flags &= 0xFFFFFDFF;
	//	PM_ExitAimDownSight_(ps);
	//}

	if ((ps->pm_flags & PMF_SIGHT_AIMING) != 0 && ps->viewHeightTarget == 11)
	{
		if (BG_WeaponNames[ps->weapon]->overlayReticle)
		{
			if (0.0f >= ps->fWeaponPosFrac)
			{
				pm->cmd.forwardmove = 0;
				pm->cmd.rightmove = 0;
			}
		}
		else
		{
			pm->cmd.forwardmove = 0;
			pm->cmd.rightmove = 0;
		}
	}
	ps->eFlags &= 0xFFFFFFBF;

	if ((ps->pm_flags & PMF_RESPAWNED) == 0)
	{
		if ((!ps->weaponstate || ps->weaponstate == 5) && ps->ammoclip[BG_WeaponNames[ps->weapon]->iClipIndex + 4] && (pm->cmd.buttons & 1) != 0)
			ps->eFlags |= EF_FIRING;
	}
	if (ps->pm_type < PM_DEAD && (pm->cmd.buttons & 0x101) == 0)
		ps->pm_flags &= 0xFFFFFBFF;

	int serverTime = pm->cmd.serverTime;
	int deltaTime = serverTime - ps->commandTime;
	//pml->msec = glob_pml->msec;
	//pml->frametime = glob_pml->frametime;

	//if (!pml->msec) {
	//	pml->msec = 3;
	//	pml->frametime = (float)pml->msec / 1000;
	//}
	pml->msec = deltaTime;
	if (deltaTime >= 1)                         // mark1
	{
		if (deltaTime > 200)
		{
			pml->msec = 200;
		}
	}
	else
	{
		pml->msec = 1;
	}
	ps->commandTime = serverTime;

	pml->previous_origin[0] = ps->origin[0];
	pml->previous_origin[1] = ps->origin[1];
	pml->previous_origin[2] = ps->origin[2];
	pml->previous_velocity[0] = ps->velocity[0];
	pml->previous_velocity[1] = ps->velocity[1];
	pml->previous_velocity[2] = ps->velocity[2];

	//pml->msec = 3; //simulate fps
	//pml->frametime = (float)pml->msec * 0.001f;
	pm->cmd.serverTime += pml->msec;
	pm->ps->commandTime += pml->msec;
	//pm->cmd.buttons = options->hold_buttons;

	float deltaX = 0;
	float deltaY = 0;
	float deltaZ = 0;

	if(controller->strafebot){
		if (auto v = CG_GetOptYawDelta(pm, pml))
			deltaY = v.value();
	}else{
		deltaX = controller->turnrate.x;
		deltaY = controller->turnrate.y;
		deltaZ = controller->turnrate.z;
	}
		

	pm->ps->viewangles[PITCH] += deltaX;
	pm->ps->viewangles[YAW] += deltaY;
	pm->ps->viewangles[ROLL] += deltaZ;

	pm->cmd.angles[PITCH] += ANGLE2SHORT(deltaX);
	pm->cmd.angles[YAW] += ANGLE2SHORT(deltaY);
	pm->cmd.angles[ROLL] += ANGLE2SHORT(deltaZ);

	PM_AdjustAimSpreadScale_(pm, pml);
	PM_UpdateViewAngles(ps, pml->msec, &pm->cmd, pm->handler);

	AngleVectors(pm->ps->viewangles, pml->forward, pml->right, pml->up); //set viewangles

	char fwd = pm->cmd.forwardmove;
	if (fwd >= 0)
	{
		if (fwd > 0 || pm->cmd.rightmove)
			ps->pm_flags &= 0xFFFFFFDF;
	}
	else
	{
		ps->pm_flags |= PMF_BACKWARDS_RUN;
	}

	if ((ps->pm_flags & PMF_MANTLE) == 0) {
		PM_UpdateAimDownSightFlag_(pm, pml);
		PM_UpdateSprint_(pm);
		PM_UpdatePlayerWalkingFlag_(pm);
		PM_CheckDuck_(pm, pml);
		PM_GroundTrace_(pm, pml);
	}

	Mantle_Check(pm, pml);

	if ((ps->pm_flags & PMF_MANTLE) != 0) {

		if ((ps->pm_flags & PMF_LADDER) != 0)
			ps->pm_flags = ps->pm_flags & 0xFFFFFFF7 | PMF_LADDER_FALL;
		ps->groundEntityNum = 1023;
		pml->groundPlane = 0;
		pml->walking = 0;

		PM_UpdateAimDownSightFlag_(pm, pml);
		PM_UpdateSprint_(pm);
		PM_UpdatePlayerWalkingFlag_(pm);
		PM_CheckDuck_(pm, pml);
		if (mantle_enable->current.enabled)
			Mantle_Move(ps, pm,pml);

		PM_Weapon(pm, pml);

	}
	else {



		PM_UpdatePronePitch(pml, pm);
		PM_DropTimers(ps, pml);
		//if ( ps->pm_type >= PM_DEAD && pml->walking )
		//	PM_DeadMove(ps);
		PM_CheckLadderMove_(pm, pml);

		if ((ps->pm_flags & PMF_LADDER) != 0)
		{
			PM_LadderMove_(pm, pml);
		}
		else if (pml->walking) {
			HookTable::getInstance().find(hookEnums_e::HOOK_PM_WALKMOVE).value()->second.cast_call<void(*)(pmove_t*, pml_t*)>(pm, pml);//walkmove
		}
		else {
			engine_call<void __cdecl>(false, 0x40F680, pm, pml); //airmove
		}
		PM_GroundTrace_(pm, pml); //call groundtrace after 
		PM_Footsteps_(pm, pml);
		PM_Weapon(pm, pml);
		//PM_FoliageSnd_(pm);
		PM_OverBounce(pm, pml);
		Sys_SnapVector(pm->ps->velocity); //Sys_SnapVector | not called in singleplayer
	}

	memcpy(&pm->oldcmd, &pm->cmd, sizeof(pm->oldcmd));
}
void PM_MovementSimulation::PM_OverBounce(pmove_t* pm, pml_t* pml)
{
	vec3_t move;

	move[0] = pm->ps->origin[0] - pml->previous_origin[0];
	move[1] = pm->ps->origin[1] - pml->previous_origin[1];
	move[2] = pm->ps->origin[2] - pml->previous_origin[2];

	float dot = move[2] * move[2] + move[1] * move[1] + move[0] * move[0];
	float dot_div_frametime = dot / (pml->frametime * pml->frametime);
	float dot_speed = pm->ps->velocity[2] * pm->ps->velocity[2] + pm->ps->velocity[1] * pm->ps->velocity[1] + pm->ps->velocity[0] * pm->ps->velocity[0];

	if (dot_speed * 0.25 > dot_div_frametime)
	{
		//Com_Printf(CON_CHANNEL_OBITUARY, "possible overbounce!\n");
		float inGameFramesPerSecond = 1.0 / pml->frametime;
		pm->ps->velocity[0] = inGameFramesPerSecond * move[0];
		pm->ps->velocity[1] = inGameFramesPerSecond * move[1];
		pm->ps->velocity[2] = inGameFramesPerSecond * move[2];
	}

	float clampedFrametime = std::clamp(pml->frametime, 0.f, 1.f);

	float diffX = pm->ps->velocity[0] - pm->ps->oldVelocity[0];
	float diffY = pm->ps->velocity[1] - pm->ps->oldVelocity[1];

	float frameX = clampedFrametime * diffX;
	float frameY = clampedFrametime * diffY;

	pm->ps->oldVelocity[0] = pm->ps->oldVelocity[0] + frameX;
	pm->ps->oldVelocity[1] = pm->ps->oldVelocity[1] + frameY;


	return;
}