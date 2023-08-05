#pragma once

#include "pch.hpp"

namespace _fs = std::filesystem;

namespace fs
{
	enum class fileopen : int
	{
		FILE_IN = 1,
		FILE_OUT = 2,
		FILE_APP = 8,
		FILE_BINARY = 32
	};
	std::string exe_file_name();
	std::string exe_path();
	std::string root_path();
	std::string get_extension(const std::string& path);
	std::string previous_directory(std::string& directory);

	bool io_open(std::ifstream& fp, const std::string& path, const fileopen type);
	void io_close(std::ifstream& f);

	void create_file(const std::string& path);
	bool create_directory(const std::string& path);



	inline bool directory_exists(const std::string& d) { return _fs::exists(d); }
	inline bool file_exists(const std::string& f) { return _fs::exists(f); }

}