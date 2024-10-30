#include "utils.h"

std::string getExecutablePath()
{
	char ep[PATH_MAX];

	#if __APPLE__
		if (NDBUG) {
			// MacOS release executable path
			CFURLRef resourceURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
			CFURLGetFileSystemRepresentation(resourceURL, true,
											(UInt8 *)ep,
											PATH_MAX);
			if (resourceURL != NULL)
				CFRelease(resourceURL);
			return std::string(ep);
		} else {
			// MacOS debug executable path
			uint32_t bufsize = PATH_MAX;
			_NSGetExecutablePath(ep, &bufsize);
		}
	#endif

	#if __linux__
		// linux path to the executable
		if (readlink("/proc/self/exe", ep, PATH_MAX) == -1)
			return std::string("");
	#endif

	#if _WIN64
		HMODULE hModule = GetModuleHandle(nullptr);
		if (GetModuleFileName(hModule, (wchar_t*)ep, MAX_PATH) == 0) {
			return std::string("");
		}
	#endif

	char slash = '/';
#if _WIN64
	slash = '\\';
#endif
	std::string binaryPath(ep);
	std::size_t lastPathIndex = binaryPath.length();
	for (std::size_t i = binaryPath.length(); i > 0; i--) {
		if (binaryPath[i] == slash) {
			lastPathIndex = i;
			break;
		}
	}

	char executablePath[PATH_MAX];
	binaryPath.copy(executablePath, lastPathIndex, 0);
	executablePath[lastPathIndex] = '\0';
	return std::string(executablePath);
}