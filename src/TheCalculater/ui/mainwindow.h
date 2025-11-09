/**
 * @file mainwindow.h
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
#pragma once

#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class VMainWindow;
}
QT_END_NAMESPACE

class VMainWindow : public QWidget {
    Q_OBJECT

public:
    VMainWindow(QWidget* parent = nullptr);
public slots:
    void on_dBtnLog_clicked();

private:
    std::unique_ptr<Ui::VMainWindow> ui;
};
