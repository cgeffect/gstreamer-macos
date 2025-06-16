
#include "view/AppLanuch.h"
#include <filesystem>
#include <cstdlib>

namespace vleap {

AppLanuch::AppLanuch(/* args */) {
}

void AppLanuch::appLanuch(std::string jsonPath) {
    std::filesystem::path homeDir(std::getenv("HOME"));
    std::filesystem::path desktopPath = homeDir / "Desktop";

    // 设置 QMainWindow 的初始大小
    mainWindow.resize(1000, 600);
    homeWidget = new vleap::HomeViewController(&mainWindow);
    mainWindow.setCentralWidget(homeWidget);
    mainWindow.show();

    homeWidget->init(jsonPath);
}

AppLanuch::~AppLanuch() {
    delete homeWidget;
}

} // namespace vleap
