
#include "FileUtil.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <fstream>
#include <sstream>

namespace vleap {

std::unique_ptr<FILE, void (*)(FILE *)> makeFileGuard(const char *str, const char *mode) {
    return std::unique_ptr<FILE, void (*)(FILE *)>(fopen(str, mode), [](FILE *file) { 
        if (file) fclose(file); });
}

std::optional<std::string> loadFromFile(const char *filePath) {
    if (!filePath || !filePath[0]) {
        printf("FilePath should not be empty\n");
        return std::optional<std::string>();
    }

    std::ifstream ifs(filePath);
    if (!ifs.is_open() || !ifs.good()) {
        printf("Failed to open file %s\n", filePath);
        return std::optional<std::string>();
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();

    return std::optional<std::string>(ss.str());
}

// 0 succ
int createDirectory(const std::string &dir) {
    if (dir.size() == 0) return -1;
    return mkdir(dir.c_str(), 0777);
}

long getFileSize(const std::string &path) {
    long size = 0;
    auto pFile = makeFileGuard(path.c_str(), "rb");
    if (pFile) {
        fseek(pFile.get(), 0, SEEK_END);
        size = ftell(pFile.get());
    }
    return size;
}

// 判断是否为目录
bool isDir(const char *path) {
    auto dir = opendir(path);
    if (!dir) {
        return false;
    }
    closedir(dir);
    return true;
}

// 判断是否为常规文件
bool fileExist(const char *path) {
    auto fp = fopen(path, "rb");
    if (!fp) {
        return false;
    }
    fclose(fp);
    return true;
}

// 判断是否是特殊目录
bool isSpecialDir(const char *path) {
    return strcmp(path, ".") == 0 || strcmp(path, "..") == 0;
}

std::string getCurrentDate() {
    std::string str;
    std::string format = "%Y%m%d";
    time_t t = time(0);
    char ch[128];
    strftime(ch, sizeof(ch), format.c_str(), localtime(&t));
    str = ch;
    return str;
}

std::string getFileExtension(const std::string &filePath) {
    std::string extension;
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos) {
        if (pos + 1 > filePath.size() - 1) {
            return "";
        }
        extension = filePath.substr(pos + 1);
    }
    return extension;
}

} // namespace avmedia
