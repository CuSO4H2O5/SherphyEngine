#include <string>
#include <vector>
#include <fstream>


namespace Sherphy 
{
	class SPVReader
	{
	public:
		static std::vector<char> readFile(const std::string& filename) {
			std::ifstream file(filename, std::ios::ate | std::ios::binary);
			SHERPHY_EXCEPTION_IF_FALSE(file.is_open(), "failed to open file!");

			size_t file_size = static_cast<size_t>(file.tellg());
			std::vector<char> buffer(file_size);

			file.seekg(0);
			file.read(buffer.data(), file_size);
			file.close();

			return buffer;
		}
	};
}