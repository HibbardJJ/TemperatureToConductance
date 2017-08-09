#ifndef FILE_SYSTEM_ACCESS
#define FILE_SYSTEM_ACCESS

#include <string>
#include <vector>

using Image = std::vector<std::vector<double>>;

namespace FileSystemAccess {
Image readImage(std::string filePath);
};

#endif
