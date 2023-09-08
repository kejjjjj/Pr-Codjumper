#pragma once

#include "pch.hpp"


namespace T::Movement
{
	bool StrafingRight(pmove_t* pm);
	bool StrafingLeft(pmove_t* pm);

	void T_Strafebot(pmove_t* pm, pml_t* pml);
	int32_t T_GetIdealFPS(pmove_t* pm, pml_t* pml);

	inline bool T_Ready() noexcept { return pm_glob && pml_glob && pm_glob->ps; }

}