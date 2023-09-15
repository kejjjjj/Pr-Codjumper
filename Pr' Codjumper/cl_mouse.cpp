#include "pch.hpp"


void __cdecl do_stuff(clientActive_t* client, float* my, float* mx)
{


	double v3; // st7
	int v4; // ecx
	int v5; // [esp+0h] [ebp-4h]

	static dvar_s* m_filter = Dvar_FindMalleableVar("m_filter");
	if (m_filter->current.enabled)
	{
		v5 = client->mouseDy[0] + client->mouseDy[1];
		*mx = (client->mouseDx[0] + client->mouseDx[1]) * 0.5;
		v3 = 0.5 * v5;
	}
	else
	{
		v4 = client->mouseIndex;
		*mx = client->mouseDx[v4];
		v3 = client->mouseDy[v4];
	}
	client->mouseIndex ^= 1u;
	*my = v3;

	client->mouseDx[client->mouseIndex] = 0;
	client->mouseDy[client->mouseIndex] = 0;
	


	PlayerMouse::get().delta = { *mx, *my };

	return;

}

__declspec(naked) void CL_GetMouseMovement(clientActive_t* client, float* my, float* mx)
{
	static clientActive_t* o1;
	static float* o2, *o3;
	__asm
	{
		//mov client, eax;
		//mov my, edx;
		//mov mx, esi;
		mov o1, eax;
		mov o2, edx;
		mov o3, esi;
		push esi;
		push edx;
		push eax;
		
		call do_stuff;

		add esp, 0xC;

		mov eax, o1;
		mov edx, o2;
		mov esi, o3;
	}

	__asm retn;
	//__asm mov client, eax;
	//__asm mov my, edx;
	//__asm mov mx, esi;



	//decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_CL_MOUSEMOVE);


	//if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
	//	std::cout << "yippee!\n";
	//}
	////void* _ptr = detour_func.get_ptr();

	////__asm
	////{
	////	mov eax, client;
	////	mov edx, my;
	////	mov esi, mx;
	////	call _ptr;
	////}
	////return;
	//return detour_func.cast_call<void(*)(clientActive_t*, float*, float*)>(client, my, mx);
}