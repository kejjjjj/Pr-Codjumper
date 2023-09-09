#include "pch.hpp"

void brush_geom_find_with_filter(const bool newState)
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
void brush_geom_change_filter(bool)
{
	brushWindings.clear();
	brush_geom_find_with_filter(true);
}
void terrain_geom_find_with_filter(const bool newState)
{

	if (!newState) {
		cm_terrainpoints.clear();
		Com_Printf(CON_CHANNEL_OBITUARY, "^2clearing!\n");
		return;
	}

	const auto filter = find_evar<char>("Filter##01");

	for (int i = 0; i < cm->numLeafs; i++) {
		CM_GetTerrainTriangles(&cm->leafs[i], filter->get_array());

	}


	//Com_Printf(CON_CHANNEL_OBITUARY, "^2%s -> [%s]\n", filter->get_array(), cm->materials[cm_terrainpoints.front().aabb->materialIndex].material);


}
void terrain_geom_change_filter(bool)
{
	cm_terrainpoints.clear();
	terrain_geom_find_with_filter(true);
}
void Gui::geometry_create_hardcoded()
{
	decltype(auto) resources = Resources::getInstance();

	Gui_MainCategory category(resources.FindTexture("clipmap").value(), "Clip Map");
	Gui_SubCategory subcategory("Brushes");
	Gui_SubCategory subcategory_terrain("Terrain");
	Gui_SubCategory subcategory_preferences("Preferences");

	geometry_create_clipmap(subcategory);
	geometry_create_terrain(subcategory_terrain);
	geometry_create_preferences(subcategory_preferences);

	append_category(category);
	categories.back().append_subcategory(subcategory);
	categories.back().append_subcategory(subcategory_terrain);
	categories.back().append_subcategory(subcategory_preferences);

}

void Gui::geometry_create_clipmap(Gui_SubCategory& category)
{
	char coll_buf[128] = "clip";
	EvarTable& instance = EvarTable::getInstance();

	Gui_CategoryItems items("Brushes");

	const auto showcollision = instance.add_variable<bool>("Show Collisions", false);
	const auto showcollision_filter = instance.add_array<char>("Filter", coll_buf, 128);


	items.append_item(Gui_Item(showcollision, "Draws the outlines of each collision brush", brush_geom_find_with_filter, std::nullopt, true));
	items.append_item(Gui_Item(showcollision_filter, "Only draws materials that include this substring", brush_geom_change_filter, std::nullopt, false, true));

	category.append_itemlist(items);




}
void Gui::geometry_create_terrain(Gui_SubCategory& category)
{
	EvarTable& instance = EvarTable::getInstance();
	Gui_CategoryItems items("Terrain");
	char coll_buf[128] = "clip";


	const auto showterrain = instance.add_variable<bool>("Show Terrain", false);
	const auto showterrain_filter = instance.add_array<char>("Filter##01", coll_buf, 128);

	items.append_item(Gui_Item(showterrain, "Draws the outlines of each terrain piece", terrain_geom_find_with_filter, std::nullopt, true));
	items.append_item(Gui_Item(showterrain_filter, "Only draws materials that include this substring", terrain_geom_change_filter, std::nullopt, false, true));

	category.append_itemlist(items);

}
void Gui::geometry_create_preferences(Gui_SubCategory& category)
{
	EvarTable& instance = EvarTable::getInstance();

	Gui_CategoryItems items("General");


	const auto only_bounces = instance.add_variable<bool>("Only Bounces", false);
	const auto depth_test = instance.add_variable<bool>("Depth Test", false);

	items.append_item(Gui_Item(only_bounces, "Only renders windings that can be bounced", std::nullopt, std::nullopt, true));
	items.append_item(Gui_Item(depth_test, "Don't render through walls", std::nullopt, std::nullopt, true));

	category.append_itemlist(items);

}