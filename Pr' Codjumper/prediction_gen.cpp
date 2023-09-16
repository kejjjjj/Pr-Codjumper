#include "pch.hpp"

PredictionSegment::PredictionSegment(pmove_t* pm, pml_t* pml, const int _serverTime) : serverTime(_serverTime)
{
	if (pm && pml) {
		memcpy(&initial_state.pm, pm, sizeof(pmove_t));
		memcpy(&initial_state.pml, pml, sizeof(pml_t));
		memcpy(&initial_state.ps, pm->ps, sizeof(playerState_s));
		initial_state.pm.ps = &initial_state.ps;

		memcpy(&state.pm, &initial_state.pm, sizeof(pmove_t));
		memcpy(&state.pml, &initial_state.pml, sizeof(pml_t));
		memcpy(&state.ps, &initial_state.ps, sizeof(playerState_s));
		state.pm.ps = &state.ps;
	}

	controller = std::move(std::unique_ptr<prediction_controller>(new prediction_controller));

	cmds.resize(controller->numInputs);

	controller->turntype = std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn());


	//refresh_prediction(nullptr);
}

void PredictionSegment::resize_segment(int32_t newSize) noexcept
{
	if (newSize < 1)
		newSize = 1;

	cmds.clear();
	controller->numInputs = newSize;
	cmds.resize(controller->numInputs);
	//std::cout << "newSize: " << cmds.size() << '\n';
	
	//refresh_prediction(nullptr);



}
void PredictionSegment::add_frames(int32_t newSize) 
{ 
	resize_segment(controller->numInputs + newSize); 
}
void PredictionSegment::remove_frames(int32_t newSize) 
{ 
	resize_segment(controller->numInputs - newSize); 
}
void PredictionSegment::refresh_prediction(const prediction_state* previous_state)
{
	if (previous_state) {
		memcpy(&initial_state.pm, &previous_state->pm, sizeof(pmove_t));
		memcpy(&initial_state.pml, &previous_state->pml, sizeof(pml_t));
		memcpy(&initial_state.ps, previous_state->pm.ps, sizeof(playerState_s));
		initial_state.pm.ps = &initial_state.ps;
		serverTime = initial_state.pm.cmd.serverTime;
	}
	memcpy(&state.pm, &initial_state.pm, sizeof(pmove_t));
	memcpy(&state.pml, &initial_state.pml, sizeof(pml_t));
	memcpy(&state.ps, &initial_state.ps, sizeof(playerState_s));
	state.pm.ps = &state.ps;

	pmove_t* pm = &state.pm;
	

	for (size_t i = 0; i < cmds.size(); i++) {
		const auto results = PmoveSingleSimulation(&state.pm, &state.pml, controller.get());



		VectorCopy((int*)&results.cmd_angles, cmds[i].angles);
		cmds[i].buttons = results.buttons;
		cmds[i].forwardmove = results.forwardmove;
		cmds[i].rightmove = results.rightmove;
		cmds[i].FPS = results.FPS;
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

void PredictionSegment::Hud_DrawPath(bool active_segment)
{
	if (cmds.empty())
		return;

	auto points = [this]() {
		std::list<std::optional<std::pair<ivec2, bool>>> points;
		float velocity = 0;
		float oldvelocity = 0;

		for (const auto& i : cmds) {
			if (auto xy = WorldToScreen(i.origin)) {
				velocity = fvec2(i.velocity.x, i.velocity.y).MagSq();
				points.push_back(std::make_pair(xy.value(), velocity >= oldvelocity));
				oldvelocity = velocity;
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

		if (*it != std::nullopt && *it2 != std::nullopt) {
			if(active_segment)
				ImGui::GetBackgroundDrawList()->AddLine((*it).value().first, (*it2).value().first, IM_COL32(255, 255, 255, 255), 1.5f);
			else
				ImGui::GetBackgroundDrawList()->AddLine((*it).value().first, (*it2).value().first, (*it).value().second ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255), 1.5f);


		}
		++it;
		++it2;
	}


}
void PredictionSegment::Hud_DrawFrameData()
{

	auto frame = &cmds[activeFrame];
	char buffer[256];

	if (auto xy = WorldToScreen(frame->origin)) {
		sprintf_s(buffer, "XY Speed: %.6f\nX: %.6f\nY: %.6f\nTime: %i", fvec2(frame->velocity.x, frame->velocity.y).mag(), frame->origin.x, frame->origin.y, frame->serverTime);
		R_AddCmdDrawTextWithEffects(buffer, "fonts/normalfont", float(xy.value().x), float(xy.value().y), 1.25f, 1.25f, 0, vec4_t{1,1,1,1}, 3, vec4_t{0,0,0,1}, 0, 0, 0, 0, 0, 0);
	}

}

bool PredictionSegment::UI_FrameEditor()
{
	static int add_or_remove_frames_count = 100;
	bool result = false;
	static DWORD ms = 0;
	ImGui::PushItemWidth(325);
	ImGui::SliderInt_22("Frame", &activeFrame, 0, controller->numInputs-1);

	ImGui::SameLine();

	ImGui::Button("<##02", ImVec2(30, 0));
	if (ImGui::IsItemActive()) {
		if (activeFrame - 1 > 0 && ms + 100 < Sys_MilliSeconds()) {
			activeFrame--;
			ms = Sys_MilliSeconds();
		}
	}
	ImGui::SameLine();
	ImGui::Button(">##02", ImVec2(30, 0));
	if (ImGui::IsItemActive()) {
		if (activeFrame + 1ull < cmds.size() && ms + 100 < Sys_MilliSeconds()) {
			activeFrame++;
			ms = Sys_MilliSeconds();

		}
	}

	ImGui::PushItemWidth(130);
	ImGui::InputInt("Count", &add_or_remove_frames_count, 1, 100);

	ImGui::SameLine();
	if (ImGui::Button("Add##01")) {
		add_frames(add_or_remove_frames_count);
		result = 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove##01")) {
		remove_frames(add_or_remove_frames_count);
		result = 1;
	}
	ImGui::NewLine();
	ImGui::Separator();

	return result;
}
bool PredictionSegment::UI_OtherControls()
{
	bool result = false;
	ImGui::BeginGroup();

	constexpr const char* viewangle_options[] = { "Fixed turn", "Strafebot" };

	ImGui::PushItemWidth(150);
	if (ImGui::Combo("Angles", &(int&)controller->turntype_enum, viewangle_options, IM_ARRAYSIZE(viewangle_options))) {

		if (controller->turntype)
			controller->turntype.reset();

		switch (controller->turntype_enum) {
		case prediction_angle_enumerator::FIXED_TURN:
			controller->turntype = std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn());
			break;
		case prediction_angle_enumerator::STRAFEBOT:
			controller->turntype = std::unique_ptr<prediction_viewangle_strafebot>(new prediction_viewangle_strafebot());
			break;
		}
		result = true;
		//refresh_each_segment();
	}
	ImGui::NewLine();
	result += UI_AngleControls_Fixed();
	result += UI_AngleControls_Strafebot();

	ImGui::EndGroup();

	result += UI_ControlsDPAD();

	ImGui::NewLine();

	ImGui::PushItemWidth(150);
	if (ImGui::InputInt("FPS##02", &controller->FPS)) {
		result = 1;
		controller->FPS = std::clamp(controller->FPS, 1, 1000);
	}

	ImGui::SameLine();
	if (ImGui::Checkbox2("AutoFPS", &controller->autofps))
		result = 1;

	return result;

}
bool PredictionSegment::UI_AngleControls_Fixed()
{
	bool result = false;

	if (controller->turntype_enum != prediction_angle_enumerator::FIXED_TURN)
		return result;

	auto fixed = dynamic_cast<prediction_viewangle_fixed_turn*>(controller->turntype.get());

	ImGui::PushItemWidth(150);
	if (ImGui::InputFloat("Right##01", &fixed->right, 0.f, 0.f, "%.3f"))
		result = 1;


	ImGui::PushItemWidth(150);
	if (ImGui::InputFloat("Up##01", &fixed->up, 0.f, 0.f, "%.3f"))
		result = 1;


	return result;

}
bool PredictionSegment::UI_AngleControls_Strafebot()
{
	bool result = false;
	if (controller->turntype_enum != prediction_angle_enumerator::STRAFEBOT)
		return result;

	const auto strafebot = dynamic_cast<prediction_viewangle_strafebot*>(controller->turntype.get());

	ImGui::PushItemWidth(150);
	if (ImGui::InputFloat("Up##02", &strafebot->up, 0.f, 0.f, "%.3f")) {
		strafebot->up = std::clamp(strafebot->up, -85.f, 85.f);
		result = true;
	}

	return result;
}
bool PredictionSegment::UI_ControlsDPAD()
{
	auto& fwd = controller->forwardmove;
	auto& rmove = controller->rightmove;
	bool result = 0;

	const auto constructkey = [this, &result](char& dir, char value, const char* name) {
		
		if (dir == value)
			ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.f, 0.f, 0.f, 0.7f);

		if (ImGui::Button(name, ImVec2(40, 40))) {

			if (dir == value)
				dir = NULL;
			else
				dir = value;

			result = 1;

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

	return result;
}
bool PredictionSegment::UI_Buttons()
{
	static std::vector<std::pair<std::string, int>> cmd_buttons
	{
		std::make_pair("+attack", cmdEnums::fire),
		std::make_pair("+melee", cmdEnums::melee),
		std::make_pair("+use", cmdEnums::activate),
		std::make_pair("+reload", cmdEnums::reload),
		std::make_pair("+leanleft", cmdEnums::lean_left),
		std::make_pair("+lean_right", cmdEnums::lean_right),
		std::make_pair("+prone", cmdEnums::prone),
		std::make_pair("+crouch", cmdEnums::crouch),
		std::make_pair("+jump", cmdEnums::jump),
		std::make_pair("+sprint", cmdEnums::sprint),
		std::make_pair("+frag", cmdEnums::frag),
		std::make_pair("+special", cmdEnums::special_grenade),
		std::make_pair("+ads_hold", cmdEnums::ads_hold)
	};


	bool result = 0;
	ImGui::Text("Hold Buttons");
	static bool show_actions = false;
	int items = 0;
	int32_t& hold_buttons = controller->buttons;

	for (auto& [x, y] : cmd_buttons) {

		if ((hold_buttons & y) != 0) {
			items++;
			ImGui::Text(x.c_str());

			if ((items % 4 == 0 && items != 0) == false)
				ImGui::SameLine();
		}
	}

	if (ImGui::Button("+"))
		show_actions = !show_actions;

	const ImVec2 location = ImGui::GetItemRectMax();

	if (!show_actions)
		return result;

	static ImVec2 mins, maxs, size = ImGui::GetWindowSize(), pos = ImGui::GetWindowPos();
	auto framepadding = ImGui::GetStyle().FramePadding;
	ImGui::Begin("##01", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
	bool clicked = ImGui::GetIO().MouseClicked[0];
	
	for (auto& [x, y] : cmd_buttons) {

		ImGui::Text(x.c_str());

		mins = ImVec2(pos.x, ImGui::GetItemRectMin().y);
		maxs = ImVec2(pos.x + size.x, ImGui::GetItemRectMax().y);

		if (MouseHovered(mins, maxs)) {
			ImGui::GetForegroundDrawList()->AddRectFilled(mins, maxs, IM_COL32(255, 200, 157, 157), ImGui::GetStyle().FrameRounding);
			if (clicked) {

				((hold_buttons & y) == 0) == true ?
					hold_buttons += y :
					hold_buttons -= y;

				show_actions = false;

				result = 1;
			}
		}

		if ((hold_buttons & y) != 0) {
			ImGui::GetForegroundDrawList()->AddRectFilled(mins, maxs, IM_COL32(255, 200, 157, 157), ImGui::GetStyle().FrameRounding);
		}

		if (clicked && show_actions) { //clicked out of frame
			show_actions = false;
		}

		

	}
	size = ImGui::GetWindowSize();
	pos = ImGui::GetWindowPos();
	ImGui::SetWindowPos(ImVec2(location.x + framepadding.x, location.y - framepadding.y * 3 - size.y));
	ImGui::End();
	
	return result;

}