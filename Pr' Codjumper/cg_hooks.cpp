#include "pch.hpp"

void CG_CreatePermaHooks()
{
	decltype(auto) hooktable = HookTable::getInstance();
	decltype(auto) renderer = Renderer::getInstance();


	hooktable.insert(hookEnums_e::HOOK_SHUTDOWN_RENDERER, hook::hookobj<void*>(0x46CA40, renderer.CL_ShutdownRenderer, TRUE));
	hooktable.insert(hookEnums_e::HOOK_RECOVER_LOST_DEVICE, hook::hookobj<void*>(0x5F5360, renderer.R_RecoverLostDevice, TRUE));
	hooktable.insert(hookEnums_e::HOOK_WNDPROC, hook::hookobj<void*>(is_cod4x() ? is_cod4x() + 0x801D6 : 0x57BB20, renderer.WndProc, TRUE));


	hooktable.insert(hookEnums_e::HOOK_SV_MAP, hook::hookobj<void*>(0x55D3D0, SV_Map, TRUE));
	hooktable.insert(hookEnums_e::HOOK_CL_DISCONNECT, hook::hookobj<void*>(0x4696B0, CL_Disconnect, TRUE));


}
void CG_CreateHooks()
{

	decltype(auto) hooktable = HookTable::getInstance();
	decltype(auto) renderer = Renderer::getInstance();

	hooktable.insert(hookEnums_e::HOOK_ENDSCENE,			hook::hookobj<void*>((renderer.endscene_getter()),	renderer.EndSceneRenderer,		TRUE));
	hooktable.insert(hookEnums_e::HOOK_DRAWACTIVE,			hook::hookobj<void*>(is_cod4x() ? is_cod4x() + 0x5464 : 0x42F7F0, renderer.CG_DrawActive, TRUE));
	
	GScript::getInstance().create_hooks(hooktable);

	hooktable.insert(hookEnums_e::HOOK_PM_WALKMOVE, hook::hookobj<void*>(0x40F7A0, PM_WalkMove, TRUE));
	hooktable.insert(hookEnums_e::HOOK_PM_AIRMOVE, hook::hookobj<void*>(0x40F680, PM_AirMove, TRUE));
	hooktable.insert(hookEnums_e::HOOK_PM_UPDATE_VIEWANGLES, hook::hookobj<void*>(0x413580, PM_UpdateViewAngles, TRUE));

	hooktable.insert(hookEnums_e::HOOK_RB_ENDSCENE, hook::hookobj<void*>(0x658860, RB_DrawDebug, TRUE));

	hooktable.insert(hookEnums_e::HOOK_CL_MOUSEMOVE, hook::hookobj<void*>(0x463400, CL_GetMouseMovement, TRUE));
	hooktable.insert(hookEnums_e::HOOK_CL_FINISHMOVE, hook::hookobj<void*>(0x463A60, CL_FinishMove, TRUE));


}
void CG_ReleaseHooks()
{
	decltype(auto) hooktable = HookTable::getInstance();
	decltype(auto) table = const_cast<std::unordered_map<hookEnums_e, hook::hookobj<void*>>&>(hooktable.getter().get());

	auto it = table.begin();
	std::cout << "table size: " << table.size() << '\n';
	for (auto& i : table) {
		++it;
		if (i.first < hookEnums_e::HOOK_ENDSCENE)
			continue;

		if(const_cast<hook::hookobj<void*>&>(i.second).release() == false) {
			FatalError("failed to release hook!\n");
		}

		table.erase(i.first);

	}
	std::cout << "table size: " << table.size() << '\n';
	std::cout << "hooks released!\n";
}
inline namespace hook__ {
	static hook::hookobj<void*> empty_hook;
}
hook::hookobj<void*>& find_hook(const hookEnums_e hook)
{
	decltype(auto) _hook = HookTable::getInstance().find(hook);
	
	if (!_hook.has_value()) {
		FatalError(std::format("find_hook(): failed to find hook index {}\n", std::underlying_type_t<hookEnums_e>(hook)));
		return hook__::empty_hook;
	}

	return const_cast<hook::hookobj<void*>&>(_hook.value()->second);


}