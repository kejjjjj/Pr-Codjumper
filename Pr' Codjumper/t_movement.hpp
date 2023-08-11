#pragma once

#include "pch.hpp"

namespace T::Movement
{
	void T_Strafebot(pmove_t* pm, pml_t* pml);
	void T_CorrectOverstrafe(pmove_t* pm, pml_t* pml);
}