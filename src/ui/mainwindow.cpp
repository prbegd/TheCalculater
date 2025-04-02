#include "mainwindow.h"
#include "spdlog/pch.h"

VMainWindow::VMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::VMainWindow>())
{
    SPDLOG_TRACE("函数进入");
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/resources/icon.ico"));
    SPDLOG_TRACE("函数退出");
}
