#include  "pch.hpp"
void Gui::visuals_create_hardcoded()
{
	decltype(auto) resources = Resources::getInstance();

	Gui_MainCategory category(resources.FindTexture("visuals").value(), "Visuals");
	Gui_SubCategory subcategory("Anglehelpers");

	visuals_create_strafes(subcategory);

	append_category(category);
	active_category->append_subcategory(subcategory);
	
}
void Gui::visuals_create_strafes(Gui_SubCategory& category)
{
	EvarTable& instance = EvarTable::getInstance();

	Gui_CategoryItems items("Strafing");
	
	const auto aimtrainer = instance.add_variable<bool>("Aim trainer", false);
	items.append_item(Gui_Item(aimtrainer, "creates a ball that you have to look at with your cursor", std::nullopt, true));
	category.append_itemlist(items);



}