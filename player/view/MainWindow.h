#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        // 创建菜单栏
        QMenuBar *menuBar = new QMenuBar(this);
        setMenuBar(menuBar);

        // 创建文件菜单
        QMenu *fileMenu = menuBar->addMenu(tr("&File"));

        // 添加退出选项
        QAction *exitAction = fileMenu->addAction(tr("&Exit"));
        exitAction->setShortcut(QKeySequence::Quit); // 设置快捷键
        connect(exitAction, &QAction::triggered, this, &MainWindow::exitApplication);

        // 创建帮助菜单
        QMenu *helpMenu = menuBar->addMenu(tr("&Help"));

        // 添加关于选项
        QAction *aboutAction = helpMenu->addAction(tr("&About"));
        connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);

        this->setMenuWidget(menuBar);

        this->menuBar()->setNativeMenuBar(true);
    }

private slots:
    void exitApplication() {
        QMessageBox::information(this, tr("Exit"), tr("Are you sure you want to exit?"));
        QApplication::quit();
    }

    void showAboutDialog() {
        QMessageBox::about(this, tr("About"), tr("This is a sample Qt application."));
    }
};
