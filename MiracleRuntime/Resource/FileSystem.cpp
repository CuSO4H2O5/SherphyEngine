#include "FileSystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Sherphy 
{
	std::vector<char> FileSystem::readBinaryFile(const char* filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		SHERPHY_EXCEPTION_IF_FALSE(file.is_open(), "failed to open file!");

		size_t file_size = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);
		file.close();

		return buffer;
	}

	unsigned char* FileSystem::readImageFile(const char* filename, int& tex_width, int& tex_height, int& tex_channels)
	{
		stbi_uc* pixels = stbi_load(filename, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

		SHERPHY_RETURN_NULLPTR_IF_FALSE_WITH_LOG_ERROR(pixels, "failed to load texture image " + std::string(filename));

		return static_cast<unsigned char*>(pixels);
	}

	void FileSystem::releaseImageAsset(unsigned char* asset)
	{
		return stbi_image_free(static_cast<stbi_uc*>(asset));
	}

	void FileSystem::loadObjFile(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const char* model_path)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		SHERPHY_EXCEPTION_IF_FALSE(tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path), warn + err);

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.tex_coord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.find(vertex) == uniqueVertices.end()) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
 


}