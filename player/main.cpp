#include <QtWidgets/QApplication>

#include "view/AppLanuch.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    vleap::AppLanuch appLanuch;
    appLanuch.appLanuch("/Users/jason/Jason/mogic/Mac/native-render/res/protocol/catton.json");

    return app.exec();
}
