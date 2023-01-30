#include "widget.h"

//#include "processenv.h"
#include "windows.h"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QString userName = QDir::home().dirName();
    const QString saveDir = "C:/Users/" + userName + "/.etkinlikKayit";

    QDir d;
    if(!d.exists(saveDir))
    {
        if(!d.mkpath(saveDir))
            qDebug() << "klasor olusturulamadi";
        else
        {
            std::vector<TCHAR> Buffer(saveDir.length() + 1);
            saveDir.toWCharArray(Buffer.data());
            Buffer[saveDir.length()] = '\0';
            SetCurrentDirectory(Buffer.data());
        }
    }

    Widget w;
    w.show();
    return a.exec();
}
