#include "FileManager.h"

FileManager* FileManager::instance = nullptr;

FileManager::FileManager() { ; }
FileManager::~FileManager() { ; }

FileManager* FileManager::get()
{
	if (instance == nullptr) {
		instance = new FileManager();
		return instance;
	}
	else
		return instance;
}

std::vector<char> FileManager::readFile(const std::string filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
