#include "pch.hpp"


void CreateCommands_f()
{
	Cmd_AddCommand("pr_open_menu", Gui::getInstance().menu_toggle);


}
void DeleteCommands_f()
{
	Cmd_RemoveCommand("pr_open_menu");
}