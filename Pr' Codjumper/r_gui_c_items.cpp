
#include "pch.hpp"


void Gui_CategoryItems::render()
{
	render_titlebar();

}
void Gui_CategoryItems::render_titlebar()
{

	ImGui::BeginChild("huuuh", ivec2(200, 400), true);
	
	auto window = ImGui::GetCurrentWindow();
	const ImRect title_bar_rect = window->TitleBarRect();
	ivec2 p = title_bar_rect.Min;
	auto wnd = ImGui::FindWindowByName("cj")->DrawList;
	const auto& color = ImGui::GetStyle().Colors[ImGuiCol_Separator];
	const auto c = ImColor(color.x, color.y, color.z, color.w);

	wnd->AddRectFilled(title_bar_rect.Min, ivec2(title_bar_rect.Max.x, title_bar_rect.Max.y + 32), IM_COL32(33, 33, 33, 255), ImGui::GetStyle().WindowRounding);

	wnd->AddRectFilled(
		ivec2(p.x, p.y + 32),
		ivec2(title_bar_rect.Max.x, p.y + 32 + 3),
		c);

	ImGui::GetWindowDrawList()->AddText(ivec2(title_bar_rect.Min.x + 10, title_bar_rect.Min.y), IM_COL32(255, 255, 255, 150), "yooooooooooooo!");
	ImGui::EndChild();
}