#pragma once

#include <string>
#include <vector>

#define RANDOM_PATH_LENGTH 10
#define RANDOM_DIR_PERMISSIONS 0700

namespace Routing {
    namespace Utils {
        std::vector<std::string> FindFilesByExtension(std::string path, std::string extension);

        std::string CreateRandomDirectory(std::string path);

        std::string RandString(int length);

        bool CheckFileExists(std::string fileName);

        bool RemoveDirectory(std::string path);

    }
}

