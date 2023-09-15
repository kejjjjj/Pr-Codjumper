#pragma once

#include "pch.hpp"

class Prediction_Init
{
public:
	Prediction_Init() = default;
	//Prediction_Init(const std::string& mapname);

	void Init(const std::string& mapname);

	static Prediction_Init& getInstance() { static Prediction_Init r; return r; }
	static void UI_ToggleRenderState();
	bool UI_ShouldRender() const noexcept { return is_loaded ? prediction->UI_ShouldRender() : rendering; }
	void UI_Render();
	void Hud_Render();
	bool ProjectExists() const noexcept(true) { return is_loaded; }

	auto get_playback() { return prediction->convert_to_playback(); }
	auto get_entry_state() { return prediction->get_entry_state(); }
private:
	void UI_SelectProject();
	void UI_CreateNew();

	bool is_loaded = false;
	bool empty_directory = false;
	bool rendering = false;

	void close() noexcept;
	void open() noexcept;


	std::list<std::string> projects;

	std::unique_ptr<Prediction> prediction;

	Prediction_Init(const Prediction_Init&) = delete;
	Prediction_Init& operator=(const Prediction_Init&) = delete;
};