#pragma once

#include "pch.hpp"

class Gui
{
public:
	static Gui& getInstance() { static Gui g; return g; }

	static void menu_toggle();
	void initialize();
	void reset();

	void render();

	//returns the maxs
	void left_categories();
	void bottom_categories();
	void right_categories();

	void append_category(const Gui_MainCategory& category) { categories.push_back(category); if (categories.size() == 1) active_category = &categories.front(); };

	ivec2 main_category_first_image_position(const size_t numimages, float spacing) const noexcept;
	void set_active(Gui_MainCategory& c) noexcept { active_category = &c; }
private:

	void visuals_create_hardcoded();
	void visuals_create_strafes(Gui_SubCategory& category);

	void automation_create_hardcoded();
	void automation_create_movement(Gui_SubCategory& category);

	void geometry_create_hardcoded();
	void geometry_create_clipmap(Gui_SubCategory& category);

	void render_topbar();
	void clip_bounds();

	void update_dimensions();

	void close() noexcept;
	void open() noexcept;
	bool is_open = false;

	std::list<Gui_MainCategory> categories;
	Gui_MainCategory* active_category = 0;
	ivec2 absolute_maxs;
	ivec2 free_space;
	ivec2 pos;
	ivec2 size;
	ivec2 cursor;

	ivec2 mainSize = { 50, 50 };

	ivec2 left_mins;
	ivec2 left_maxs;

};