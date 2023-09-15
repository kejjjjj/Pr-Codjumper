#pragma once

#include "pch.hpp"


class PredictionSegment
{
public:
	PredictionSegment(pmove_t* pm, pml_t* pml, const int _serverTime);
	PredictionSegment() = delete;

	void refresh_prediction();
	void resize_segment(int32_t newSize) noexcept;
	void add_frames(int32_t newSize) { resize_segment(controller.numInputs + newSize); }
	void remove_frames(int32_t newSize) { resize_segment(controller.numInputs - newSize); }

	auto get_controls() noexcept { return &controller; }
	void Hud_DrawPath();

	auto get_data() { return cmds; }
	friend class Prediction;
private:

	prediction_state state;
	prediction_state initial_state;

	prediction_controller controller;

	std::vector<playback_cmd> cmds;

	int serverTime = 0;
	PredictionSegment(const PredictionSegment&) = delete;
	PredictionSegment& operator=(const PredictionSegment&) = delete;

};
