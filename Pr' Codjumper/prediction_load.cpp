#include "pch.hpp"

//Prediction_Init::Prediction_Init(const std::string& mapname)
//{
//
//
//
//
//
//}

void Prediction_Init::Init(const std::string& mapname)
{
	if (is_loaded)
		return;

	const std::string directory = fs::root_path() + "\\prediction_runs\\" + mapname;

	if (!fs::directory_exists(directory)) {
		empty_directory = true;
		return;
	}


	projects = fs::files_in_directory(directory);

	if (projects.empty()) {
		empty_directory = true;
		return;
	}

	//remove all unrelated files from the list
	projects.erase(std::remove_if(projects.begin(), projects.end(), [](const std::string& project) {
		return project.find(".predicted") == std::string::npos;
		}), projects.end());
}
void Prediction_Init::UI_CreateNew()
{
	struct rgb {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
		uint32_t pack() const noexcept {
			return *(unsigned int*)&r;
		}

	};
	const auto cool_color = [](int width, int height) 
	{
		const auto float_to_rgb = [](float v, float min, float max){
			
			rgb coldColor = { 0,0,255 };
			rgb warmColor = { 255,0,0 };
			
			v = std::max(min, std::min(max, v));
			float factor = (v - min) / (max - min);

			rgb result;
			result.r = coldColor.r + static_cast<uint8_t>(factor * (warmColor.r - coldColor.r));
			result.g = coldColor.g + static_cast<uint8_t>(factor * (warmColor.g - coldColor.g));
			result.b = coldColor.b + static_cast<uint8_t>(factor * (warmColor.b - coldColor.b));
			result.a = 255;
			return result;
		};
		ivec2 m = GetCPos();
		const rgb color = float_to_rgb(sqrt(pow(m.x,2) + pow(m.y, 2)), 0, width);
		//auto& style = ImGui::GetStyle();

		//fvec2 padding = {style.FramePadding.x * 2 + style.ItemSpacing.x, style.FramePadding.y*2 + style.ItemSpacing.y};
		//
		//ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowSize(), color.pack());

		return color;
	};
	
	static char project_name[32];
	ImGui::TextCenteredVertical("There are no existing projects for this map, so create a new one");

	auto& style = ImGui::GetStyle();

	ImGui::SetNextItemWidth(ImGui::GetWindowWidth()-style.FramePadding.x*2 - style.ItemSpacing.x);
	ImGui::InputText("##yea", project_name, 32);
	fvec2 size = ImGui::GetItemRectSize();

	auto c = cool_color(cgs->refdef.width, cgs->refdef.height);

	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorConvertU32ToFloat4(c.pack()));

	if(ImGui::ButtonCentered("Create")){
		if (!fs::valid_file_name(project_name)) {
			Com_Printf(CON_CHANNEL_OBITUARY, "^1Error: not a valid file name\n");
			return;
		}

		is_loaded = true;


		prediction = std::unique_ptr<Prediction>(new Prediction(std::string(project_name), pm_glob, pml_glob));
		close();
	}

	ImGui::PopStyleColor();

}
void Prediction_Init::UI_SelectProject()
{
	const fvec2 rect_offs = { cgs->refdef.width / 6.f, cgs->refdef.height / 7.68f };
	const ivec2 mins(cgs->refdef.width / 2 - rect_offs.x, cgs->refdef.height / 2 - rect_offs.y);
	const ivec2 maxs(cgs->refdef.width / 2 + rect_offs.x, cgs->refdef.height / 2 + rect_offs.y);

	ImGui::SetNextWindowPos(mins);
	ImGui::SetNextWindowSize(rect_offs * 2);
	ImGui::Begin("Select Project", &rendering, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	const ImGuiIO& io = ImGui::GetIO();

	if (!rendering || ImGui::IsKeyPressed(VK_ESCAPE))
		close();

	if (empty_directory)
		return UI_CreateNew();

	ImGui::End();
	

}
void Prediction_Init::UI_Render()
{
	if (is_loaded)
		return prediction->UI_Render();

	if (!rendering)
		return;

	return UI_SelectProject();



}
void Prediction_Init::UI_ToggleRenderState()
{ 

	auto& r = getInstance(); 

	if (r.is_loaded)
		return r.prediction->UI_ToggleRenderState();

	r.rendering = !r.rendering; 

	if (!r.rendering)
		return r.close();

	if (!r.is_loaded)
		r.Init(Dvar_FindMalleableVar("mapname")->current.string);
	
	r.open();

}

void Prediction_Init::close() noexcept
{
	IN_ActivateMouse(true);
	UI_SetSystemCursorPos(320.f, 240.f);
	IN_ActivateKeyboard(true);
	ImGui::GetIO().MouseDrawCursor = false;
	rendering = false;
}
void Prediction_Init::open() noexcept
{
	IN_ActivateMouse(false);
	UI_SetSystemCursorPos(320.f, 240.f);
	IN_ActivateKeyboard(false);

	ImGui::GetIO().MouseDrawCursor = true;
	rendering = true;
}
void Prediction_Init::Hud_Render() {
	if (!ProjectExists())
		return;

	prediction->Hud_Render();

}