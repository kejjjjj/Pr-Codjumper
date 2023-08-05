#pragma once

#include "pch.hpp"

inline uintptr_t is_cod4x() {
	static const HMODULE cod4x_handle = GetModuleHandle("cod4x_021.dll");
	return reinterpret_cast<uintptr_t>(cod4x_handle);
}

template<typename Return, typename ... Args>
inline Return engine_call(const bool cod4x, const uintptr_t offset, Args... args)
{
	static const HMODULE cod4x_handle = GetModuleHandle("cod4x_021.dll");
	return (reinterpret_cast<Return(*)(Args...)>(cod4x ? reinterpret_cast<uintptr_t>(cod4x_handle) + offset : offset))(args...);
}

void CBuf_Addtext(const char* text);

cmd_function_s* Cmd_FindCommand(const char* name);
cmd_function_s* Cmd_AddCommand(char* cmdname, void(__cdecl* function)());
cmd_function_s* Cmd_RemoveCommand(const char* cmdname);