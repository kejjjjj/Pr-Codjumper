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

	const auto aimtrainer = instance.add_variable<bool>("Strafebot", false);
	items.append_item(Gui_Item(aimtrainer, "this doesn't do anything yet!", std::nullopt, true));
	category.append_itemlist(items);


}