/**
 * @file mainwindow.h
 * @author prbegd
 * @brief Mainwindow UI
 * @date 2025-05-25
 * 
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
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

private:
    std::unique_ptr<Ui::VMainWindow> ui;
};
