#ifndef UTIL_FILESYSTEM_H
#define UTIL_FILESYSTEM_H

#include <string>

namespace RBX {

class FileSystem
{
public:
	static std::string getUserAppDataDirectory(bool create, const char* subDirectory);
	static std::string getCacheDirectory(bool create);
	static std::string getContentDirectory();
	static void setAndroidBasePaths(
		const std::string& filesDir, const std::string& cacheDir, const std::string& contentDir
	);
};

} // namespace RBX

#endif // UTIL_FILESYSTEM_H
