#include "widget.h"

//#include "processenv.h"
//#include "windows.h"

#include <QApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QString userName = QDir::home().dirName();
#ifdef _WIN32
	const QString saveDir = "C:/Users/" + userName + "/.etkinlikKayit";
#else
	const QString saveDir = "/home/" + userName + "/.etkinlikKayit";
#endif

    QDir d;
    if(!d.exists(saveDir))
    {
        if(!d.mkpath(saveDir))
            qDebug() << "klasor olusturulamadi";
        else
        {
//            std::vector<TCHAR> Buffer(saveDir.length() + 1);
//            saveDir.toWCharArray(Buffer.data());
//            Buffer[saveDir.length()] = '\0';
//            SetCurrentDirectory(Buffer.data());
			QDir::setCurrent(saveDir);
        }
    }

    Widget w;
    w.show();
    return a.exec();
}
