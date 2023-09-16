#pragma once

#include "pch.hpp"



class Prediction
{
public:
	explicit Prediction(const std::string& full_path, const std::string& name, pmove_t* pm, pml_t* pml);
	Prediction(const std::string& path); // load existing
	void Hud_Render();
	void Hud_EngineRender();
	void UI_Render();
	bool UI_ShouldRender() const noexcept { return ui_rendering; }
	void UI_ToggleRenderState();

	void SaveToFile();

	std::list<playback_cmd> convert_to_playback();
	playerState_s get_entry_state() { return predicted.front()->initial_state.ps; }
private: 
	std::vector<std::unique_ptr<PredictionSegment>> predicted;
	PredictionSegment* current_segment;
	friend class PredictionSegment;
	//UI
	void UI_SegmentEditor();

	void Hud_Hitbox();

	template<typename T>
	void IO_WriteData(std::ofstream& f, const T& data, bool brackets=true);
	void IO_WriteData(std::ofstream& f, char* const begin, char* const end);
	void IO_WriteController(std::ofstream& f, prediction_controller& c);

	void IO_ReadVersion(std::fstream& f);
	void IO_ReadController(std::fstream& f, std::unique_ptr<prediction_controller>& c);
	template<typename T>
	std::optional<T> IO_ReadBlock(std::fstream& f, size_t amount_of_bytes = 0);
	void IO_ReadBlock(std::fstream& f, size_t amount_of_bytes, void* dst);

	void refresh_each_segment() noexcept;

	void add_segment();
	void insert_segment();
	void delete_segment();

	bool ui_rendering = false;
	void ui_close() noexcept;
	void ui_open() noexcept;

	std::string project_name;
	std::string project_full_path;
	int32_t current_segment_index;

	DWORD last_edit_time = 0;


	Prediction(const Prediction&) = delete;
	Prediction& operator=(const Prediction&) = delete;
};