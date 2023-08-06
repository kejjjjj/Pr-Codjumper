#pragma once

#include "pch.hpp"
class Gui_Item
{

};


class Gui_CategoryItems
{
public:
	Gui_CategoryItems(const std::string& _title) : title(_title) {};

	void render();

private:

	void render_titlebar();

	std::string title;
	std::list<Gui_Item> items;

};