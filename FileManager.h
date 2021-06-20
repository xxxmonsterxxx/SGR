#pragma once

#include "utils.h"

class ShaderManager;

class FileManager {
	friend class ShaderManager;

private:
	FileManager();
	~FileManager();
	FileManager(const FileManager&) = delete;
	FileManager operator=(FileManager&) = delete;

	static FileManager* instance;

	static std::vector<char> readFile(const std::string filename);
public:
	static FileManager* get();
};
