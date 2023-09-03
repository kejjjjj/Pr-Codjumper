#include "pch.hpp"

void geom_find_with_filter(const bool newState)
{

	if (!newState) {
		brushWindings.clear();
		Com_Printf(CON_CHANNEL_OBITUARY, "^2clearing!\n");
		return;
	}

	const auto filter = find_evar<char>("Filter");

	for (int i = 0; i < cm->numBrushes; i++) {

		if (std::string(cm->materials[cm->brushes[i].axialMaterialNum[0][0]].material).find(filter->get_array()) == std::string::npos)
			continue;

		CM_GetPolys(&cm->brushes[i]);

	}

	Com_Printf(CON_CHANNEL_OBITUARY, "^2%s!\n", filter->get_array());


}
void geom_change_filter(bool)
{
	brushWindings.clear();
	geom_find_with_filter(true);
}

void Gui::geometry_create_hardcoded()
{
	decltype(auto) resources = Resources::getInstance();

	Gui_MainCategory category(resources.FindTexture("clipmap").value(), "Clip Map");
	Gui_SubCategory subcategory("Collisions");

	geometry_create_clipmap(subcategory);

	append_category(category);
	categories.back().append_subcategory(subcategory);
}

void Gui::geometry_create_clipmap(Gui_SubCategory& category)
{
	char coll_buf[128] = "clip";
	EvarTable& instance = EvarTable::getInstance();

	Gui_CategoryItems items("Strafing");

	const auto showcollision = instance.add_variable<bool>("Show Collisions", false);
	const auto showcollision_filter = instance.add_array<char>("Filter", coll_buf, 128);
	const auto only_bounces = instance.add_variable<bool>("Only Bounces", false);
	const auto depth_test = instance.add_variable<bool>("Depth Test", false);

	items.append_item(Gui_Item(showcollision, "Draws the outlines of each collision brush", geom_find_with_filter, std::nullopt, true));
	items.append_item(Gui_Item(showcollision_filter, "Only draws materials that include this substring", geom_change_filter, std::nullopt, false, true));
	items.append_item(Gui_Item(only_bounces, "Only renders windings that can be bounced", std::nullopt, std::nullopt, true));
	items.append_item(Gui_Item(depth_test, "Don't render through walls", std::nullopt, std::nullopt, true));

	category.append_itemlist(items);
}