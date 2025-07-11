#include <QApplication>
#include <qmessagebox.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // if (argc != 2) return 2;
    // QString crashReportPath = argv[1];
    QWidget *widget = new QWidget;
    widget->show();
    
    return app.exec();
}