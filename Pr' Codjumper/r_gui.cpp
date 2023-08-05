#include "pch.hpp"

void Gui::menu_toggle()
{
	decltype(auto) gui = getInstance();
	gui.is_open = !gui.is_open;

	if (gui.is_open)
		return gui.open();

	gui.close();

}
void Gui::close() noexcept
{
	IN_ActivateMouse(true);
	UI_SetSystemCursorPos(320.f, 240.f);
	ImGui::GetIO().MouseDrawCursor = false;
	is_open = false;
}
void Gui::open() noexcept
{
	IN_ActivateMouse(false);
	UI_SetSystemCursorPos(320.f, 240.f);
	ImGui::GetIO().MouseDrawCursor = true;
	is_open = true;
}
void Gui::reset()
{
	close();
}
void Gui::render()
{
	if (!is_open)
		return;

	ImGui::Begin("Codjumper tools [https://github.com/kejjjjj/Pr-Codjumper]", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar/*ImGuiWindowFlags_AlwaysAutoResize*/);

/*	ImGui::Text("HELLLLLLLLOOOOOOOOOOOOOOOOOO");

	static bool whooooo = false;
	ImGui::Checkbox2("BITCH", &whooooo)*/;
	auto& style = ImGui::GetStyle();


	render_topbar();
	left_categories();
	bottom_categories();

	ImGui::End();

}


void Gui::render_topbar()
{
	
	ImGui::Text("free space: (%ipx, %ipx)", free_space.x, free_space.y);
	ImGui::Dummy(ivec2(0, 50));
	ImGui::Separator();

	update_dimensions();
	clip_bounds();
}
void Gui::left_categories()
{
	auto& style  = ImGui::GetStyle();
	ImVec2 mins = ImVec2(pos.x + cursor.x - style.WindowPadding.x * 4, pos.y + cursor.y - style.WindowPadding.y / 2 + 1);
	ImVec2 maxs = ImVec2(pos.x + cursor.x + 150, pos.y + cursor.y + free_space.y - 100);



	ImGui::BeginChild("##11asd", ImVec2(maxs.x - mins.x, maxs.y - mins.y), true);

	ImGui::GetWindowDrawList()->AddRectFilled(
		mins,
		ImVec2(maxs.x + style.FramePadding.x * style.WindowPadding.x, maxs.y), 
		IM_COL32(33, 33, 33, 255));

	ImGui::BeginGroup();

	ImGui::Text("Angles");

	ImGui::EndGroup();

	ImGui::EndChild();

	//float height = ImGui::GetItemRectSize().y;

	//ImGui::Dummy(ImVec2(maxs.x - mins.x, maxs.y - mins.y - height));

	//ImGui::EndChild();
	

	update_dimensions();
	clip_bounds();
}
void Gui::bottom_categories()
{
	ImGui::Separator();

}
void Gui::update_dimensions()
{
	pos = ImGui::GetWindowPos();
	size = ImGui::GetWindowSize();
	cursor = ImGui::GetCursorPos();
	absolute_maxs = ivec2(cgs->refdef.width, cgs->refdef.height);
	free_space = ivec2(size.x - cgs->refdef.x - cursor.x, size.y - cgs->refdef.y - cursor.y);
	//free_space -= cursor;
}
void Gui::clip_bounds()
{
	if (size.x > absolute_maxs.x)
		ImGui::SetWindowSize(ivec2(absolute_maxs.x, size.y));
	if (size.y > absolute_maxs.y)
		ImGui::SetWindowSize(ivec2(size.x, absolute_maxs.y));

	if (pos.x < NULL)
		ImGui::SetWindowPos(ivec2(0, pos.y));
	if (pos.y < NULL)
		ImGui::SetWindowPos(ivec2(pos.x, 0));

	else if (pos.x + size.x > absolute_maxs.x)
		ImGui::SetWindowPos(ivec2(absolute_maxs.x - size.x, pos.y));
	else if (pos.y + size.y > absolute_maxs.y)
		ImGui::SetWindowPos(ivec2(pos.x, absolute_maxs.y - size.y));
}