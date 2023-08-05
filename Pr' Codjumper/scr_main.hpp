#pragma once

#include "pch.hpp"


typedef int scr_entref_t;
typedef void(__cdecl* xmethod_t)(scr_entref_t);
typedef void(*xfunction_t)();


class GScript
{

public:;

	GScript() = default;

	static GScript& getInstance() { static GScript g; return g; }

	void initialize();

	bool insert_function(const char* cmd_name, xfunction_t function, bool developer);
	bool insert_method(const char* cmd_name, xfunction_t function, bool developer);
	void create_hooks(HookTable& table);
private:
	
	static xmethod_t Player_GetMethod(const char** name);
	static xfunction_t Scr_GetFunction(const char** name, int* type);

	
	void add_methods();
	void add_functions();

	std::list<scr_function_s> scr_functions;
	std::list<scr_function_s> scr_methods;


};