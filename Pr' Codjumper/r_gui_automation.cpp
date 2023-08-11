#include "pch.hpp"

void Gui::automation_create_hardcoded()
{

	decltype(auto) resources = Resources::getInstance();

	Gui_MainCategory category(resources.FindTexture("automation").value(), "Automation");
	Gui_SubCategory subcategory("Inputs");

	automation_create_movement(subcategory);

	append_category(category);
	categories.back().append_subcategory(subcategory);

}
void Gui::automation_create_movement(Gui_SubCategory& category)
{
	EvarTable& instance = EvarTable::getInstance();

	Gui_CategoryItems items("Strafing");

	const auto strafebot = instance.add_variable<bool>("Strafebot", false);
	const auto overstrafe_correction = instance.add_variable<bool>("Overstrafe Correction", true);


	items.append_item(Gui_Item(strafebot, "optimal acceleration", std::nullopt, true));
	items.append_item(Gui_Item(overstrafe_correction, "blocks the player from overstrafing", std::nullopt, true));

	category.append_itemlist(items);


}