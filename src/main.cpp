#include <string>
#ifdef WIN32
# include "init/windows.hpp"
#else
# include "init/posix.hpp"
#endif
#include "mainwindow.h"

#include <QMessageBox>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    init(argc, argv);
    VMainWindow w; 
    QMessageBox::information(nullptr, "来自ccompiler深处的回响", "“" + QString::number(__cplusplus)  + "”");
    w.show();
    return a.exec();
}