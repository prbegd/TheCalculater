#include "mainwindow.h"

VMainWindow::VMainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(std::make_unique<Ui::VMainWindow>())
{
    SPDLOG_TRACE("Enter func");
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/resources/icon.ico"));
    SPDLOG_TRACE("Leave func");
}
