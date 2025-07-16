#include "crash_handler_dialog.hpp"
#include <QCoreApplication>

namespace TheCalculater::crash_handler {
    CrashHandlerDialog::CrashHandlerDialog(const QString &crashReportFile, QWidget* parent)
        : QDialog(parent), ui(std::make_unique<Ui::CrashHandlerDialog>())
    {
        ui->setupUi(this);
        ui->restartBtn->setFocus();
        setTabOrder({ ui->restartBtn, ui->reportBtn, ui->logBtn, ui->closeBtn });

        ui->messageText->setText(ui->messageText->text().arg(QCoreApplication::applicationDirPath() + "/" + crashReportFile));
    }
}