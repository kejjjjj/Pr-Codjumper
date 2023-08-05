#pragma once

#include "pch.hpp"


class Resources
{
public:
	Resources() = default;

	static Resources& getInstance() { static Resources r; return r; }

	void initialize();

	std::optional<ImFont*> FindFont(const std::string& name);

private:
	
	void create_subdirectory(const std::string& name) const noexcept;
	void load_font(const char* name, const std::string& file_name, const float scale);

	std::vector<std::pair<ImFont*, const char*>> fonts;

};