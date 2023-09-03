#pragma once

#include "pch.hpp"
class Gui_Item
{
public:
	union _slider
	{
		struct {
			int min;
			int max;
		}integer;
		struct {
			float min;
			float max;
		}value;
	};
	Gui_Item(EvarBase* ref, const char* _tooltip, std::optional<std::function<void(bool)>> onClicked=std::nullopt, std::optional<std::shared_ptr<_slider>> sliderData = std::nullopt, bool isCheckbox = false, bool isInputbox = false);

	void render();

private:
	EvarBase* linked_variable = 0;
	bool checkbox = false;
	bool inputbox = false;
	std::shared_ptr<_slider> slider;
	const char* tooltip;
	std::function<void(bool)> activation_event;
};


class Gui_CategoryItems
{
public:
	Gui_CategoryItems(const std::string& _title) : title(_title) {};

	void render();

	void append_item(const Gui_Item& i) { items.push_back(i); }

private:

	void render_titlebar();
	void render_body();
	std::string title;
	std::list<Gui_Item> items;

	ivec2 child_mins;
	ivec2 body_mins;
};