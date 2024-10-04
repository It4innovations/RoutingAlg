#include "FilesystemUtil.h"
#include <dirent.h>
#include <regex>
#include <unistd.h>
#include <sys/stat.h>
#include <random>
#include <iostream>
#include <sstream>
#include <cstring>

namespace Routing {
    namespace Utils {
        std::vector<std::string> FindFilesByExtension(std::string path, std::string extension) {
            DIR *dirp = opendir(path.c_str());
            struct dirent *entry;
            std::vector<std::string> output;

            if (!dirp) {
                std::cerr << "Cannot open directory " << path << std::endl;
                return output;
            }

            while ((entry = readdir(dirp)) != nullptr) {
                if (entry->d_type == DT_REG) {
                    int length = strlen(entry->d_name);
                    std::string entrystr = std::string(entry->d_name, length);
                    std::stringstream regstring;
                    // Matches filenames with given extension
                    regstring << "(\\." << extension << ")$";
                    std::smatch matches;
                    bool found = std::regex_search(entrystr, matches, std::regex(regstring.str()));
                    if (found) {
                        output.push_back(entry->d_name);
                    }
                }
            }
            closedir(dirp);
            return output;
        }

        std::string CreateRandomDirectory(std::string path) {
            std::string tempDir(path + "/" + RandString(RANDOM_PATH_LENGTH));
            while (CheckFileExists(tempDir)) {
                tempDir = path + "/" + RandString(RANDOM_PATH_LENGTH);
            }
            mkdir(tempDir.c_str(), RANDOM_DIR_PERMISSIONS);
            return tempDir;
        }

        std::string RandString(int length) {
            std::string output;
            output.resize(length);
            std::default_random_engine eng;
            std::random_device rnd;
            eng.seed(rnd());
            std::uniform_int_distribution<int> dist('a', 'z');
            for (int i = 0; i < length; ++i) {
                output[i] = dist(eng);
            }
            return output;
        }

        bool CheckFileExists(std::string fileName) {
            return (access(fileName.c_str(), R_OK) == F_OK);
        }

        bool RemoveDirectory(const std::string path) {
            DIR *dir = opendir(path.c_str());
            if (dir == NULL) {
                int errcode = errno;
                switch (errcode) {
                    case EACCES:
                        std::cerr << "Permission denied for directory: " << path << std::endl;
                        break;
                    case ENOENT:
                        std::cerr << "Directory does not exist: " << path << std::endl;
                        break;

                    case ENOTDIR:
                        std::cerr << "Not a directory: " << path << std::endl;
                        break;
                    default:
                        std::cerr << "Unknown error - errno: " << errcode << std::endl;
                        break;
                }
                return false;
            } else {
                struct dirent *d;
                bool success = true;
                while (success && (d = readdir(dir))) {
                    // Skip dots
                    std::string currentEntry(d->d_name);
                    if (currentEntry == "." || currentEntry == "..") {
                        continue;
                    }
                    // If current entry is file, delete it.
                    // If current entry is directory, recursively delete
                    std::string newEntry = path + "/" + currentEntry;
                    struct stat statbuf;
                    if (!stat(newEntry.c_str(), &statbuf)) {
                        if (S_ISDIR(statbuf.st_mode)) {
                            success = RemoveDirectory(newEntry);
                        } else {
                            int err = unlink(newEntry.c_str());
                            success = (err == 0);
                        }
                    } else {
                        int errcode = errno;
                        std::cerr << "Error opening: " << newEntry << " errno " << errcode << std::endl;
                    }
                }
                closedir(dir);
                rmdir(path.c_str());
                return success;
            }
        }


    }
}


