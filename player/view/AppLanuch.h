
#pragma once
#include <QtWidgets/QMainWindow>

#include <stdio.h>
#include <iostream>

#include <functional>

#include "view/vc/HomeViewController.h"
#include "view/MainWindow.h"

namespace vleap {
class AppLanuch {
private:
public:
    AppLanuch(/* args */);
    ~AppLanuch();
    MainWindow mainWindow;
    vleap::HomeViewController *homeWidget;
    void appLanuch(std::string jsonPath);
};

} // namespace vleap
