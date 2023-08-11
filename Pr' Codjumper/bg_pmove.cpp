
#include "pch.hpp"

void PM_AirMove(pmove_t* pm, pml_t* pml)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_PM_AIRMOVE);

	memcpy(&pm_glob, pm, sizeof(pmove_t));
	memcpy(&pml_glob, pml, sizeof(pml_t));

	T::Movement::T_Strafebot(pm, pml);
	T::Movement::T_CorrectOverstrafe(pm, pml);

	return detour_func.cast_call<void(__cdecl*)(pmove_t* pm, pml_t* pml)>(pm, pml);
}
void PM_WalkMove(pmove_t* pm, pml_t* pml)
{
	memcpy(&pm_glob,pm, sizeof(pmove_t));
	memcpy(&pml_glob, pml, sizeof(pml_t));

	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_PM_WALKMOVE);

	T::Movement::T_Strafebot(pm, pml);
	T::Movement::T_CorrectOverstrafe(pm, pml);

	return detour_func.cast_call<void(__cdecl*)(pmove_t * pm, pml_t * pml)>(pm, pml);
}