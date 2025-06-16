

#ifndef FileUtil_hpp
#define FileUtil_hpp

#include <stdio.h>
#include <memory>
#include <string>
#include <optional>

#define SAFE_FREE(p) \
    if (p) {         \
        free(p);     \
    }

namespace vleap {

std::unique_ptr<FILE, void (*)(FILE *)> makeFileGuard(const char *str, const char *mode);

std::optional<std::string> loadFromFile(const char *filePath);

int createDirectory(const std::string &dir);

// void deleteMogicCreateFile(const std::string &dir, const std::string &prefixName);

long getFileSize(const std::string &path);

// 判断是否为目录
bool isDir(const char *path);

// 判断是否为常规文件
bool fileExist(const char *path);

// 判断是否是特殊目录
bool isSpecialDir(const char *path);

// 计算MD5值, 失败情况返回空字符串
std::string hashMD5(const std::string &filePath);

std::string getCurrentDate();

std::string getFileExtension(const std::string &filePath);

// class PAGUtil {
// public:
//     static std::shared_ptr<pag::PAGFile> LoadPAG(const std::string &filePath,
//                                                  const std::string &password = "",
//                                                  const void *jbytes = nullptr, size_t jlength = 0);
// };

} // namespace avmedia

#endif /* FileUtil_hpp */
