#include "crash_handler_dialog.hpp"

namespace TheCalculater::crash_handler {
    CrashHandlerDialog::CrashHandlerDialog(QWidget* parent)
        : QDialog(parent), ui(std::make_unique<Ui::CrashHandlerDialog>())
    {
        ui->setupUi(this);
        ui->restartBtn->setFocus();
        setTabOrder({ ui->restartBtn, ui->reportBtn, ui->logBtn, ui->closeBtn });
    }
}