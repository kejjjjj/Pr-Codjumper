#pragma once

#include "pch.hpp"

class Gui_SubCategory
{
public:
	Gui_SubCategory(const std::string& display_text) : preview_text(display_text){}

	void render(const ivec2& pos, const ivec2& maxs, bool active, float y_offs, Gui_SubCategory** currently_active);

	void append_itemlist(const Gui_CategoryItems& c) { items.push_back(c); }
	auto& get_items() const noexcept { return items; }
	const int rect_height = 42;
private:

	void render_inactive(const ivec2& pos, const ivec2& maxs, float y_offs, Gui_SubCategory** currently_active);
	void render_active(const ivec2& pos, const ivec2& maxs, float y_offs);
	
	std::list<Gui_CategoryItems> items;

	std::string preview_text;
	
};
