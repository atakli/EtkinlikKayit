#include "widget.h"

//#include "processenv.h"
//#include "windows.h"

#include <QApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QString saveDir = QDir::home().filePath(".etkinlikKayit");

    QDir d;
    if(!d.exists(saveDir))
    {
        if(!d.mkpath(saveDir))
            qDebug() << "klasor olusturulamadi";
        else
			QDir::setCurrent(saveDir);
    }

    Widget w;
    w.show();
    return a.exec();
}
