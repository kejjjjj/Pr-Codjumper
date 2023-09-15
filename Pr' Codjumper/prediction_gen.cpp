#include "pch.hpp"

PredictionSegment::PredictionSegment(pmove_t* pm, pml_t* pml, const int _serverTime) : serverTime(_serverTime)
{
	memcpy(&initial_state.pm, pm, sizeof(pmove_t));
	memcpy(&initial_state.pml, pml, sizeof(pml_t));
	memcpy(&initial_state.ps, pm->ps, sizeof(playerState_s));
	initial_state.pm.ps = &initial_state.ps;

	state = initial_state;

	cmds.resize(controller.numInputs);

	controller.turntype = std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn());


	refresh_prediction();
}

void PredictionSegment::resize_segment(int32_t newSize) noexcept
{
	if (newSize < 1)
		newSize = 1;

	cmds.clear();
	controller.numInputs = newSize;
	cmds.resize(controller.numInputs);
	std::cout << "newSize: " << cmds.size() << '\n';
	
	refresh_prediction();
}

void PredictionSegment::refresh_prediction()
{
	memcpy(&state.pm, &initial_state.pm, sizeof(pmove_t));
	memcpy(&state.pml, &initial_state.pml, sizeof(pml_t));
	memcpy(&state.ps, &initial_state.ps, sizeof(playerState_s));
	state.pm.ps = &state.ps;

	int offs = state.pm.cmd.serverTime;

	for (size_t i = 0; i < cmds.size(); i++) {
		const auto results = PmoveSingleSimulation(&state.pm, &state.pml, &controller);

		VectorCopy((int*)&results.cmd_angles, cmds[i].angles);
		cmds[i].buttons = controller.buttons;
		cmds[i].forwardmove = controller.forwardmove;
		cmds[i].rightmove = controller.rightmove;
		cmds[i].FPS = controller.FPS;
		cmds[i].mins = results.mins;
		cmds[i].maxs = results.maxs;
		cmds[i].weapon = results.weapon;
		cmds[i].offhand = results.offhand;
		cmds[i].camera_yaw = results.camera_yaw;
		cmds[i].serverTime = state.pm.cmd.serverTime;
		cmds[i].origin = state.pm.ps->origin;
		cmds[i].velocity = state.pm.ps->velocity;

	}
}

void PredictionSegment::Hud_DrawPath()
{
	if (cmds.empty())
		return;

	auto points = [this]() {
		std::list<std::optional<ivec2>> points;
		for (const auto& i : cmds) {
			if (auto xy = WorldToScreen(i.origin)) {
				points.push_back(xy.value());
				continue;
			}
			points.push_back(std::nullopt);
		}

		return (points);
	}();

	auto it = points.begin();
	auto it2 = it;
	std::advance(it2, 1);

	auto end = points.end();


	while (it2 != end) {

		if (*it != std::nullopt && *it2 != std::nullopt)
			ImGui::GetBackgroundDrawList()->AddLine((*it).value(), (*it2).value(), IM_COL32(0, 255, 0, 255), 3);

		++it;
		++it2;
	}


}