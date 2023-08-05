#include "pch.hpp"

std::string fs::exe_file_name()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}
std::string fs::exe_path()
{
	std::string f = exe_file_name();
	return f.substr(0, f.find_last_of("\\/"));
}
std::string fs::root_path()
{
	return exe_path() + "\\Pr";
}
std::string fs::get_extension(const std::string& file)
{
	size_t const extensionPos = file.find_last_of(".");

	if (extensionPos == std::string::npos)
		return "No extension";

	return file.substr(extensionPos);
}
std::string fs::previous_directory(std::string& directory)
{
	size_t pos = directory.find_last_of('\\');
	if (pos < 1 || pos == std::string::npos)
		return directory;

	return directory.substr(0, pos);
}
bool fs::io_open(std::ifstream& fp, const std::string& path, const fs::fileopen type)
{
	if (fp.is_open())
		return true;

	fp.open(path, static_cast<std::ios_base::openmode>(std::underlying_type_t<fs::fileopen>(type)));

	if (!fp.is_open())
		return false;


	return true;
}
void fs::io_close(std::ifstream& f)
{
	if (f.is_open())
		f.close();
}

void fs::create_file(const std::string& path)
{
	std::fstream* nf = new std::fstream(path, std::ios_base::out);
	*nf << "";
	if (nf->is_open())
		nf->close();
	delete nf;
}
bool fs::create_directory(const std::string& path)
{
	return _mkdir((path).c_str()) != -1;
}
