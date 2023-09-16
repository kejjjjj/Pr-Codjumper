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
void Prediction::refresh_each_segment() noexcept
{
	auto it = predicted.begin();
	auto it2 = it;
	std::advance(it2, 1);

	predicted.front()->refresh_prediction(0);

	while (it2 != predicted.end()) {

		(*it2)->refresh_prediction(&(*it)->state);

		++it;
		++it2;
	}

	last_edit_time = Sys_MilliSeconds();

}
void Prediction::UI_Render()
{
	if (!ui_rendering)
		return;

	ImGui::Begin("Prediction Tool", &ui_rendering, ImGuiWindowFlags_AlwaysAutoResize);
	
	UI_SegmentEditor();
	if (current_segment->UI_FrameEditor())
		refresh_each_segment();
	if (current_segment->UI_OtherControls())
		refresh_each_segment();
	if (current_segment->UI_Buttons())
		refresh_each_segment();


	if (!ui_rendering || ImGui::IsKeyPressed(VK_ESCAPE))
		ui_close();

	ImGui::End();

}

void Prediction::UI_SegmentEditor()
{
	size_t num_segments = predicted.size()-1ull;
	static DWORD ms = 0;

	ImGui::PushItemWidth(350);
	if (ImGui::SliderInt_22("Segment", &current_segment_index, 0, num_segments)) {
		current_segment = predicted[current_segment_index].get();
	}

	ImGui::SameLine();

	ImGui::Button("<##01", ImVec2(30, 0));
	if (ImGui::IsItemActive()) {
		if (current_segment_index > 0 && ms + 100 < Sys_MilliSeconds()) {
			current_segment_index--;
			ms = Sys_MilliSeconds();
			current_segment = predicted[current_segment_index].get();

		}
	}
	ImGui::SameLine();
	ImGui::Button(">##01", ImVec2(30, 0));
	if (ImGui::IsItemActive()) {
		if (current_segment_index + 1ull < predicted.size() && ms + 100 < Sys_MilliSeconds()) {
			current_segment_index++;
			ms = Sys_MilliSeconds();
			current_segment = predicted[current_segment_index].get();

		}
	}
	if (ImGui::Button("Add")) {
		add_segment();
	}
	ImGui::SameLine();

	if (ImGui::Button("Insert")) {
		insert_segment();
	}
	ImGui::SameLine();

	if(current_segment_index == 0)
		ImGui::BeginDisabled();


	if (ImGui::Button("Remove Selected")) {
		delete_segment();
	}

	if (current_segment_index == 0)
		ImGui::EndDisabled();

	ImGui::NewLine();

}