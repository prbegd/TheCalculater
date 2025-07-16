#include "crash_handler_dialog.hpp"
#include <QCoreApplication>
#include <QProcess>

namespace TheCalculater::crash_handler {
    CrashHandlerDialog::CrashHandlerDialog(const QString& crashReportFile, QStringList originArgs, QWidget* parent)
        : QDialog(parent), ui(std::make_unique<Ui::CrashHandlerDialog>()), crashReportFile_(QCoreApplication::applicationDirPath() + "/" + crashReportFile), originArgs_(std::move(originArgs))
    {
        ui->setupUi(this);
        ui->restartBtn->setFocus();
        setTabOrder({ ui->restartBtn, ui->reportBtn, ui->logBtn, ui->closeBtn });

        ui->messageText->setText(ui->messageText->text().arg(crashReportFile_));
    }

    void CrashHandlerDialog::on_restartBtn_clicked()
    {
        QProcess::startDetached("./TheCalculater", originArgs_);
        qApp->quit();
    }
    void CrashHandlerDialog::on_closeBtn_clicked()
    {
    }
    void CrashHandlerDialog::on_logBtn_clicked()
    {
    }
    void CrashHandlerDialog::on_reportBtn_clicked()
    {
    }
} // namespace TheCalculater::crash_handler