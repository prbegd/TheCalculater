#include "crash_handler_dialog.hpp"

namespace TheCalculater::crash_handler {
    CrashHandlerDialog::CrashHandlerDialog(QWidget* parent)
        : QDialog(parent), ui(std::make_unique<Ui::CrashHandlerDialog>())
    {
        ui->setupUi(this);
    }
}