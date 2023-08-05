#include "pch.hpp"

LPDIRECT3DTEXTURE9 file2texture(const std::string& source)
{
	LPDIRECT3DTEXTURE9 tex;
	const auto result = D3DXCreateTextureFromFile(cg::dx->device, source.c_str(), &tex);

	if (result != S_OK) {
		FatalError(std::format("file2texture(): \"{}\" failed with {}", source, result));
		return 0;
	}

	return tex;
}