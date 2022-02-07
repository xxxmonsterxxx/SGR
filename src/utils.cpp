#include "utils.h"

std::string getExecutablePath()
{
	char ep[PATH_MAX];

	#if __APPLE__
		if (NDBUG) {
			// MacOS release executable path
			CFURLRef resourceURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
			if (CFURLGetFileSystemRepresentation(resourceURL, true,
											(UInt8 *)ep,
											PATH_MAX)) {
				if (resourceURL != NULL)
					CFRelease(resourceURL);
			}
			return std::string(ep);
		} else {
			// MacOS debug executable path
			uint32_t bufsize = PATH_MAX;
			_NSGetExecutablePath(ep, &bufsize);
		}
	#endif

	#if __linux__
		// linux path to the executable
		readlink("/proc/self/exe", ep, PATH_MAX);
	#endif

	#if __WIN64
		test
	#endif

	std::string binaryPath(ep);
	int lastPathIndex = binaryPath.length();
	for (int i = binaryPath.length(); i > 0; i--) {
		if (binaryPath[i] == '/') {
			lastPathIndex = i;
			break;
		}
	}

	char executablePath[PATH_MAX];
	binaryPath.copy(executablePath, lastPathIndex, 0);
	return std::string(executablePath);
}