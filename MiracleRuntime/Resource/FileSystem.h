#pragma once

#include "Soul/PreCompile/SoulGlobal.h"

#include <gtx/hash.hpp>

#include <vector>
#include <fstream>
#include <unordered_map>

namespace std
{
	template<> struct hash<Sherphy::Vertex> {
		size_t operator()(Sherphy::Vertex const& vertex) const
		{
			return ((hash<Sherphy::Vec3>()(vertex.pos) ^ (hash<Sherphy::Vec3>()(vertex.color) << 1)) >> 1) ^ (hash<Sherphy::Vec2>()(vertex.tex_coord) << 1);
		}
	};
}

namespace Sherphy 
{
	class FileSystem
	{
	public:
		std::vector<char> readBinaryFile(const char* filename);
		unsigned char* readImageFile(const char* filename, int& tex_width, int& tex_height, int& tex_channels);
		void releaseImageAsset(unsigned char* asset);
		void loadObjFile(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const char* model_path);
	};
}