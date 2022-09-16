#include "widget.h"

#include <QApplication>
#include <QFileInfo>

QString applicationDirPath;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    applicationDirPath = QFileInfo(argv[0]).absolutePath();
    Widget w;
    w.show();
    return a.exec();
}
