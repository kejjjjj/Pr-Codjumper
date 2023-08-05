#include "pch.hpp"

void Resources::initialize()
{
	create_subdirectory("");
	create_subdirectory("resources");
	create_subdirectory("resources\\fonts");
	create_subdirectory("resources\\images");

	load_font("conduit", "conduit_itc_light1.ttf", 24.f);

}

void Resources::create_subdirectory(const std::string& name) const noexcept
{
	const std::string full_path = fs::exe_path() + "\\Pr\\" + name;

	if (!fs::directory_exists(full_path)) {
		if (!fs::create_directory(full_path)) {
			FatalError(std::format("unable to create the following directory: '{}'", full_path));
			return;
		}
	}
}
void Resources::load_font(const char* name, const std::string& file_name, const float scale)
{
	ImGuiIO* io = &ImGui::GetIO();
	if (!io) {
		FatalError("load_font(): imgui context not yet created!");
		return;
	}

	const std::string full_path = fs::root_path() + "\\resources\\fonts";
	const std::string c_str = std::string(full_path + "\\" + file_name);

	if (!fs::file_exists(c_str)) {
		std::cout << "filename: " << c_str.c_str() << '\n';
		FatalError(std::format("unable to find the font: '{}'", c_str));
		return;
	}

	auto font = io->Fonts->AddFontFromFileTTF(c_str.c_str(), scale);

	if (!font) {
		std::cout << "font: " << std::quoted(c_str) << " does not exist!\n";
		FatalError(std::format("font '{}' does not exist!", c_str));
		return;
	}

	fonts.push_back(std::make_pair(font, name));

	std::cout << "font: " << std::quoted(name) << " added!\n";

}
std::optional<ImFont*> Resources::FindFont(const std::string& name)
{
	for (const auto& i : fonts) {
		if (!name.compare(i.second))
			return i.first;
	}

	return std::nullopt;
}