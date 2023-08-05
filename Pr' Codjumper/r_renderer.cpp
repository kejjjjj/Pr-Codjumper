#include "pch.hpp"

bool Renderer::initialize()
{
	std::this_thread::sleep_for(500ms);
	h_hWnd = FindWindow(NULL, is_cod4x() ? "Call of Duty 4 X" : "Call of Duty 4");

	if (IsWindow(h_hWnd) == false) {
		FatalError("failed to get game window handle");
		return false;
	}

	decltype(get_endscene()) endscene;

	if ((endscene = get_endscene()).has_value() == false) {
		FatalError("failed to obtain endscene pointer");
		return false;
	}

	pEndScene = endscene.value();

	pDevice = dx->device;

	initialize_imgui();

	std::cout << "endscene pointer: " << pEndScene << '\n';

	return true;

}
std::optional<endScene> Renderer::get_endscene()
{
	PVOID* vTable = *reinterpret_cast<PVOID**>(cg::dx->device);
	pEndScene = reinterpret_cast<endScene>(vTable[42]);
	return pEndScene;
}
void Renderer::initialize_imgui()
{
	if (!pDevice) {
		FatalError("!pDevice");
		return;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	std::string font_dir = "C:\\Windows\\Fonts\\Arial.ttf";
	io.Fonts->AddFontFromFileTTF(font_dir.c_str(), 12 * 2);
	io.Fonts->Build();

	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(h_hWnd);
	ImGui_ImplDX9_Init(pDevice);
}

bool Renderer::begin_frame()
{
	if (!ImGui::GetCurrentContext())
		return false;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return true;
}
void Renderer::end_frame()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
void Renderer::CL_ShutdownRenderer()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_SHUTDOWN_RENDERER);

	std::cout << "shutdown renderer!\n";
	Com_Printf(CON_CHANNEL_CONSOLEONLY, "shutting down renderer\n");

	if (ImGui::GetCurrentContext()) {
		Com_Printf(CON_CHANNEL_CONSOLEONLY, "also removing imgui context\n");
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	return detour_func.cast_call<void(*)()>();

}
void Renderer::R_RecoverLostDevice()
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_RECOVER_LOST_DEVICE);

	if (ImGui::GetCurrentContext()) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	return detour_func.cast_call<void(*)()>();

}