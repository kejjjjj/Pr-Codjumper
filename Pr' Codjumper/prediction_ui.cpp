#include "pch.hpp"

void Prediction::ui_close() noexcept
{
	IN_ActivateMouse(true);
	UI_SetSystemCursorPos(320.f, 240.f);
	IN_ActivateKeyboard(true);
	ImGui::GetIO().MouseDrawCursor = false;
	ui_rendering = false;
}
void Prediction::ui_open() noexcept
{
	IN_ActivateMouse(false);
	UI_SetSystemCursorPos(320.f, 240.f);
	IN_ActivateKeyboard(false);

	ImGui::GetIO().MouseDrawCursor = true;
	ui_rendering = true;
}
void Prediction::UI_ToggleRenderState()
{

	ui_rendering = !ui_rendering;

	if (!ui_rendering)
		return ui_close();


	ui_open();

}
void Prediction::UI_Render()
{
	if (!ui_rendering)
		return;

	ImGui::Begin("Prediction Tool", 0, ImGuiWindowFlags_AlwaysAutoResize);
	

	UI_FrameEditor();
	UI_OtherControls();

	if (!ui_rendering || ImGui::IsKeyPressed(VK_ESCAPE))
		ui_close();

	ImGui::End();

}
void Prediction::UI_SegmentEditor()
{

}
void Prediction::UI_FrameEditor()
{
	static int add_or_remove_frames_count = 100;

	ImGui::PushItemWidth(130);
	ImGui::InputInt("Count", &add_or_remove_frames_count, 1, 100);

	ImGui::SameLine();
	if (ImGui::Button("Add##01"))
		current_segment->add_frames(add_or_remove_frames_count);
	ImGui::SameLine();
	if (ImGui::Button("Remove##01"))
		current_segment->remove_frames(add_or_remove_frames_count);

	ImGui::NewLine();
	ImGui::Separator();

}
void Prediction::UI_OtherControls()
{
	const auto controls = current_segment->get_controls();

	ImGui::BeginGroup();

	constexpr const char* viewangle_options[] = { "Fixed turn" };

	ImGui::PushItemWidth(150);
	if (ImGui::Combo("Angles", &(int&)controls->turntype_enum, viewangle_options, IM_ARRAYSIZE(viewangle_options))) {

		if (controls->turntype)
			controls->turntype.reset();

		switch (controls->turntype_enum) {
		case prediction_angle_enumerator::FIXED_TURN:
			controls->turntype = std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn());
			break;

		}
		refresh_each_segment();
	}
	ImGui::NewLine();
	UI_AngleControls_Fixed();

	ImGui::EndGroup();

	UI_ControlsDPAD();

	ImGui::NewLine();

}
void Prediction::UI_AngleControls_Fixed()
{
	const auto controls = current_segment->get_controls();

	if (controls->turntype_enum != prediction_angle_enumerator::FIXED_TURN)
		return;

	auto fixed = dynamic_cast<prediction_viewangle_fixed_turn*>(controls->turntype.get());

	ImGui::PushItemWidth(150);
	if (ImGui::InputFloat("Right##01", &fixed->right, 0.f, 0.f, "%.3f"))
		refresh_each_segment();


	ImGui::PushItemWidth(150);
	if (ImGui::InputFloat("Up##01", &fixed->up, 0.f, 0.f, "%.3f"))
		refresh_each_segment();

}
void Prediction::UI_ControlsDPAD()
{
	auto& fwd = current_segment->get_controls()->forwardmove;
	auto& rmove = current_segment->get_controls()->rightmove;

	const auto constructkey = [this](char& dir, char value, const char* name) -> void {

		if (dir == value)
			ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.f, 0.f, 0.f, 0.7f);

		if (ImGui::Button(name, ImVec2(40, 40))) {

			if (dir == value)
				dir = NULL;
			else
				dir = value;

			refresh_each_segment();
		}

		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(30.f / 255, 30.f / 255, 41.f / 255, 1.f);
	};

	ImGui::SameLine();
	ImGui::Text("\t  ");
	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::Text("\t  ");
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(0.2f, 0));
	ImGui::SameLine();
	constructkey(fwd, 127, "W##01");

	constructkey(rmove, -127, "A##01");

	ImGui::SameLine();
	constructkey(fwd, -127, "S##01");

	ImGui::SameLine();
	constructkey(rmove, 127, "D##01");

	ImGui::EndGroup();
}