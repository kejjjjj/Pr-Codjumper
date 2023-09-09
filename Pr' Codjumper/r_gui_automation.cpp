#include "pch.hpp"

void Gui::automation_create_hardcoded()
{

	decltype(auto) resources = Resources::getInstance();

	Gui_MainCategory category(resources.FindTexture("automation").value(), "Automation");
	Gui_SubCategory subcategory("Inputs");
	Gui_SubCategory subcategory_fps("FPS");

	automation_create_movement(subcategory);
	automation_create_fps(subcategory_fps);

	append_category(category);
	categories.back().append_subcategory(subcategory);
	categories.back().append_subcategory(subcategory_fps);

}
void Gui::automation_create_movement(Gui_SubCategory& category)
{
	EvarTable& instance = EvarTable::getInstance();

	Gui_CategoryItems strafe_items("Strafing");

	const auto strafebot = instance.add_variable<bool>("Strafebot", false);

	strafe_items.append_item(Gui_Item(strafebot, "optimal acceleration", std::nullopt, std::nullopt, true));

	category.append_itemlist(strafe_items);


}
void Gui::automation_create_fps(Gui_SubCategory& category)
{
	EvarTable& instance = EvarTable::getInstance();

	Gui_CategoryItems fps_items("FPS");

	const auto autofps = instance.add_variable<bool>("AutoFPS", false);

	fps_items.append_item(Gui_Item(autofps, "Automatically switches to the best FPS for acceleration", std::nullopt, std::nullopt, true));

	category.append_itemlist(fps_items);

}