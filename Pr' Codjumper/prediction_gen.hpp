#pragma once

#include "pch.hpp"



class PredictionSegment
{
public:
	PredictionSegment(pmove_t* pm, pml_t* pml, const int _serverTime);
	PredictionSegment() = delete;

	void refresh_prediction(const prediction_state* previous_state);
	void resize_segment(int32_t newSize) noexcept;
	void add_frames(int32_t newSize);
	void remove_frames(int32_t newSize);
	void set_frame(int32_t newFrame) { activeFrame = std::clamp(newFrame, 0, controller->numInputs); }
	auto get_controls() noexcept { return &controller; }
	void Hud_DrawPath(bool active_segment);
	void Hud_DrawFrameData();
	void Hud_Hitbox();

	bool UI_FrameEditor();
	bool UI_OtherControls();
	bool UI_ControlsDPAD();
	bool UI_Buttons();

	bool UI_AngleControls_Fixed();
	bool UI_AngleControls_Strafebot();

	auto get_data() { return cmds; }
	friend class Prediction;
private:

	prediction_state state;
	prediction_state initial_state;

	std::unique_ptr<prediction_controller> controller;

	std::vector<playback_cmd> cmds;

	int activeFrame = 0;
	int serverTime = 0;
	PredictionSegment(const PredictionSegment&) = delete;
	PredictionSegment& operator=(const PredictionSegment&) = delete;

};