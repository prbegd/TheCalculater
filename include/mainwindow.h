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
