#pragma once

#include "pch.hpp"

void Scr_ObjectError(const char* msg, ...);
int Scr_GetInt(uint32_t param);
float Scr_GetFloat(uint32_t param);
float __cdecl Scr_GetVector(uint32_t param, float* vector);
int Scr_GetNumParam();
int Scr_AddInt(int val);
int Scr_AddFloat(float val);
float* Scr_AddVector(float* vector);
char* Scr_GetString(uint32_t parameter);
void Scr_AddString(char* string);
gentity_s* Scr_GetEntity(scr_entref_t entref);