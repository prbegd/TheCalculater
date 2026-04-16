/**
 * @file mainwindow.cpp
 * @author prbegd
 * @brief Mainwindow UI
 * @date 2025-05-25
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

import tpmm.spdlog;

VMainWindow::VMainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(std::make_unique<Ui::VMainWindow>())
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/resources/icon.ico"));
}

void VMainWindow::on_dBtnLog_clicked()
{
    spdlog::info("Hello, world!");
}
