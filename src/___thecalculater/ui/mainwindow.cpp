/**
 * @file mainwindow.cpp
 * @author prbegd
 * @brief Mainwindow UI
 * @date 2025-05-25
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
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
