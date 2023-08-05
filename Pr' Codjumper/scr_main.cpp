#include "pch.hpp"

void GScript::initialize()
{
	insert_function("weaponexists", []()->void
	{
		if (Scr_GetNumParam() != 1) {
			Scr_ObjectError("Usage: weaponExists( string )");
			Scr_AddInt(0);
			return;
		}
		char* weap = Scr_GetString(0);

		if (!weap) {
			Scr_AddInt(0);
			return;
		}
		const auto weapIdx = engine_call<int __cdecl>(false, 0x416610, weap);
		Scr_AddInt(weapIdx) > 0;

	}, false);

	insert_method("getbuttonpressed", (xfunction_t)PlayerCmd_GetButtonPressed, false);
	insert_function("sendcommand", (xfunction_t)GScr_SendCommand, false);

	//insert_method("getforwardmove", (xfunction_t)PlayerCmd_GetForwardMove, false);
	//insert_method("getrightmove", (xfunction_t)PlayerCmd_GetRightMove, false);
	//insert_method("setvelocity", (xfunction_t)PlayerCmd_SetVelocity, false);

}
void GScript::create_hooks(HookTable& table)
{
	table.insert(hookEnums_e::HOOK_GSCR_GETFUNCTION, hook::hookobj<void*>(0x4D8470, Scr_GetFunction, TRUE));
	table.insert(hookEnums_e::HOOK_GSCR_GETMETHOD, hook::hookobj<void*>(0x4C9E90, Player_GetMethod, TRUE));

}
bool GScript::insert_function(const char* cmd_name, xfunction_t function, bool developer)
{
	for (auto& i : scr_functions) {
		if (strcmp(cmd_name, i.name) == false) {
			if (function != NULL) {
				Com_PrintWarning(CON_CHANNEL_CONSOLEONLY, "Scr_AddFunction: %s already defined\n", cmd_name);
			}
			return false;
		}
	}

	scr_functions.push_back({
		.name = const_cast<char*>(cmd_name),
		.function = function,
		.developer = developer });

	return true;
}
bool GScript::insert_method(const char* cmd_name, xfunction_t function, bool developer)
{
	for (auto& i : scr_methods) {
		if (strcmp(cmd_name, i.name) == false) {
			if (function != NULL) {
				Com_PrintWarning(CON_CHANNEL_CONSOLEONLY, "Scr_AddMethod: %s already defined\n", cmd_name);
			}
			return false;
		}
	}
	
	scr_methods.push_back({ 
		.name = const_cast<char*>(cmd_name),
		.function = function, 
		.developer = developer });

	return true;


}
void GScript::add_methods()
{
}
void GScript::add_functions()
{

}

xmethod_t GScript::Player_GetMethod(const char** name)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_GSCR_GETMETHOD);

	xmethod_t this_func = detour_func.cast_call<xmethod_t(*)(const char**)>(name);
	static decltype(auto) instance = getInstance();

	if ((this_func))
		return this_func;

	if (instance.scr_methods.empty())
		return 0;

	for (const auto& i : instance.scr_methods) {
		if (std::string(i.name).compare(*name) == false) {
			this_func = reinterpret_cast<xmethod_t>(i.function);
			*name = i.name;
			Com_Printf(CON_CHANNEL_CONSOLEONLY, "^2Adding method: %s\n", i.name);
			return this_func;
		}
	}


	return 0;

}
xfunction_t GScript::Scr_GetFunction(const char** name, int* type)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_GSCR_GETFUNCTION);

	xfunction_t this_func = detour_func.cast_call<xfunction_t(*)(const char**, int*)>(name, type);
	static decltype(auto) instance = getInstance();

	if ((this_func))
		return this_func;

	if (instance.scr_functions.empty())
		return 0;

	for (const auto& i : instance.scr_functions) {
		if (std::string(i.name).compare(*name) == false) {
			this_func = reinterpret_cast<xfunction_t>(i.function);
			*name = i.name;
			Com_Printf(CON_CHANNEL_CONSOLEONLY, "^2Adding function: %s\n", i.name);
			return this_func;
		}
	}

	return 0;
}