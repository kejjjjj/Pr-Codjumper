#pragma once

#include "pch.hpp"

class Gui_Item
{

};
class Gui_CategoryItems
{

};
class Gui_SubCategory
{
public:


private:
};
class Gui_MainCategory
{
public:
	Gui_MainCategory(LPDIRECT3DTEXTURE9 icon, const std::string& previewtext) : thumbnail(icon), preview_text(previewtext){}

	void append_subcategory(const Gui_SubCategory& category);

private:
	LPDIRECT3DTEXTURE9 thumbnail;
	std::string preview_text;
	std::list<Gui_SubCategory> categories;
};

class Gui
{
public:
	static Gui& getInstance() { static Gui g; return g; }

	static void menu_toggle();
	void reset();

	void render();

	void left_categories();
	void bottom_categories();

	void append_category(const Gui_MainCategory& category);

private:

	void render_topbar();
	void clip_bounds();

	void update_dimensions();

	void close() noexcept;
	void open() noexcept;
	bool is_open = false;

	std::list<Gui_MainCategory> categories;

	ivec2 absolute_maxs;
	ivec2 free_space;
	ivec2 pos;
	ivec2 size;
	ivec2 cursor;

};