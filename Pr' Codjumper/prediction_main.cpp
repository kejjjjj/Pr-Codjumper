#include "pch.hpp"


Prediction::Prediction(const std::string& name, pmove_t* pm, pml_t* pml) :
	project_name(name)
{
	predicted.push_back(std::move(std::make_unique<PredictionSegment>(pm, pml, pm->cmd.serverTime)));
	current_segment = predicted.front().get();
}

void Prediction::Hud_Render()
{
	

	std::for_each(predicted.begin(), predicted.end(), [](std::unique_ptr<PredictionSegment>& seg)
		{
			seg->Hud_DrawPath();

		});

}
std::list<playback_cmd> Prediction::convert_to_playback()
{
	std::list<playback_cmd> cmd;

	std::for_each(predicted.begin(), predicted.end(), [&cmd](std::unique_ptr<PredictionSegment>& seg) {
		auto cmds = seg->get_data();

		for (const auto& i : cmds) {
			cmd.push_back(i);
		}

	});

	return cmd;
}