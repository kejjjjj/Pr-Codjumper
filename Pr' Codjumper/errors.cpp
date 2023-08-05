
#include "pch.hpp"

void FatalError(const std::string& errMsg)
{
	MessageBox(NULL, errMsg.c_str(), "Fatal Error!", MB_ICONERROR);

	CG_ReleaseHooks();

	//other cleanup

	exit(-1);
}