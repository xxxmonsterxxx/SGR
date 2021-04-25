#pragma once

#include "utils.h"

class FileManager {
private:
	FileManager();
	~FileManager();
	FileManager(const FileManager&) = delete;
	FileManager operator=(FileManager&) = delete;

	static FileManager* instance;

public:
	static FileManager* get();

	std::vector<char> readFile(const std::string& filename);
};
