#include "pch.hpp"


void CreateCommands_f()
{
	Cmd_AddCommand("pr_open_menu", Gui::getInstance().menu_toggle);
	Cmd_AddCommand("pr_open_builder", Prediction_Init::getInstance().UI_ToggleRenderState);
}
void DeleteCommands_f()
{
	Cmd_RemoveCommand("pr_open_menu");
	Cmd_RemoveCommand("pr_open_builder");

}