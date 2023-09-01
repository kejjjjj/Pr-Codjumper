#pragma once

#include "pch.hpp"


namespace T::Movement
{
	bool StrafingRight(pmove_t* pm);
	bool StrafingLeft(pmove_t* pm);

	void T_Strafebot(pmove_t* pm, pml_t* pml);
	void T_CorrectOverstrafe(pmove_t* pm, pml_t* pml);
}