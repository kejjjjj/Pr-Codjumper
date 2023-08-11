
#include "pch.hpp"

Gui_Item::Gui_Item(EvarBase* ref, const char* _tooltip, std::optional<std::shared_ptr<_slider>> sliderData, bool isCheckbox)
{
	if (!ref) {
		FatalError("Gui_Item(): invalid ref variable");
		return;
	}

	linked_variable = ref;
	checkbox = isCheckbox;
	tooltip = _tooltip;

	if (sliderData.has_value()) {
		slider = sliderData.value();
		return;
	}

	if (checkbox && ref->is_array()) {
		FatalError("Gui_Item(): checkbox and array at the same time");
		return;
	}

	std::cout << "ref: 0x" << ref << '\n';
	std::cout << "checkbox: " << checkbox << '\n';
}

void Gui_Item::render()
{
	if (checkbox) {
		auto o = linked_variable->get_raw();
		ImGui::Checkbox2(linked_variable->get_name().c_str(), (bool*)o);
		//ImGui::SameLine();
		ImGui::Tooltip(tooltip);
	}

	
}

void Gui_CategoryItems::render()
{

	ImGui::BeginChild("huuuh", ivec2(200, 400), true);
	child_mins = ImGui::GetWindowPos();
	render_titlebar();
	
	decltype(auto) resources = Resources::getInstance();

	const auto old_font = ImGui::GetFont();
	if (auto font = resources.FindFont(ARIAL_S))
		ImGui::SetCurrentFont(font.value());

	render_body();

	ImGui::EndChild();
	ImGui::SetCurrentFont(old_font);


}
void Gui_CategoryItems::render_titlebar()
{
	
	auto window = ImGui::GetCurrentWindow();
	const ImRect title_bar_rect = window->TitleBarRect();
	ivec2 p = title_bar_rect.Min;
	auto wnd = ImGui::FindWindowByName("cj")->DrawList;
	const auto& color = ImGui::GetStyle().Colors[ImGuiCol_Separator];
	const auto c = ImColor(color.x, color.y, color.z, color.w);

	wnd->AddRectFilled(title_bar_rect.Min, ivec2(title_bar_rect.Max.x, title_bar_rect.Max.y + 32), IM_COL32(42, 42, 42, 255), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersTop);

	wnd->AddRectFilled(
		ivec2(p.x, p.y + 32),
		ivec2(title_bar_rect.Max.x, p.y + 32 + 3),
		c);

	ImGui::GetWindowDrawList()->AddText(ivec2(title_bar_rect.Min.x + 10, title_bar_rect.Min.y + 16 - ImGui::GetFontSize()/2), IM_COL32(255, 255, 255, 255), title.c_str());
	ImGui::Dummy(ivec2(0, title_bar_rect.Max.y + 32 - title_bar_rect.Min.y));

	body_mins = ivec2(p.x, p.y + 32 + 3);
}
void Gui_CategoryItems::render_body()
{
	//static bool cb;
	//static float slider = 1;
	//static const char* items[] = { "first", "second", "third" };
	//int itemIndex = 0;

	auto wnd = ImGui::FindWindowByName("cj")->DrawList;
	wnd->AddRectFilled(body_mins, ivec2(body_mins.x + 200, body_mins.y + (400 - (body_mins.y - child_mins.y))), IM_COL32(33, 33, 33, 255), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersBottom);

	for (auto& i : items) {
		i.render();
	}


	//ImGui::Checkbox2("checkbox1", &cb);
	//ImGui::SetNextItemWidth(150);
	//ImGui::SliderFloat_2("slider1", &slider, 0, 100, "%.3f");
	//ImGui::SetNextItemWidth(150);
	//ImGui::Combo("combo", &itemIndex, items, 3);
}