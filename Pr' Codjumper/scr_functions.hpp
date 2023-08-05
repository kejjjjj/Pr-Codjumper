#pragma once

#include "pch.hpp"


void PlayerCmd_GetButtonPressed(scr_entref_t arg);
void PlayerCmd_GetForwardMove(scr_entref_t arg);
void PlayerCmd_GetRightMove(scr_entref_t arg);
void PlayerCmd_SetVelocity(scr_entref_t arg);

void GScr_SendCommand();