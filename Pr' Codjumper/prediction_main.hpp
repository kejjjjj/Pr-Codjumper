#pragma once

#include "pch.hpp"



class Prediction
{
public:
	explicit Prediction(const std::string& name, pmove_t* pm, pml_t* pml);

	void Hud_Render();
	void UI_Render();
	bool UI_ShouldRender() const noexcept { return ui_rendering; }
	void UI_ToggleRenderState();
	std::list<playback_cmd> convert_to_playback();
	playerState_s get_entry_state() { return predicted.front()->initial_state.ps; }
private:
	
	std::vector<std::unique_ptr<PredictionSegment>> predicted;
	PredictionSegment* current_segment;
	friend class PredictionSegment;
	//UI
	
	void UI_SegmentEditor();
	void UI_FrameEditor();
	void UI_OtherControls();
	void UI_ControlsDPAD();

	void UI_AngleControls_Fixed();

	void refresh_each_segment() noexcept {
		std::for_each(predicted.begin(), predicted.end(), [](std::unique_ptr<PredictionSegment>& seg) { seg->refresh_prediction();  });
	}


	bool ui_rendering = false;
	void ui_close() noexcept;
	void ui_open() noexcept;

	std::string project_name;

	Prediction(const Prediction&) = delete;
	Prediction& operator=(const Prediction&) = delete;
};