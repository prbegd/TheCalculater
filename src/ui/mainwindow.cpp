/**
 * @file mainwindow.cpp
 * @author prbegd
 * @brief Mainwindow UI
 * @date 2025-05-25
 * 
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 * 
 */
#include "TheCalculater/mainwindow.h"

VMainWindow::VMainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(std::make_unique<Ui::VMainWindow>())
{
    SPDLOG_TRACE("Enter func");
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/resources/icon.ico"));
    SPDLOG_TRACE("Leave func");
}
