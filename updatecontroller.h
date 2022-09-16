#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include "fetchtimes.h"

#include <QWidget>
#include <QObject>
#include <QDialog>

extern QString applicationDirPath;

class string
{
    QString str;
    bool operator>(string s)
    {

    }
};

class UpdateController : public QObject
{
    Q_OBJECT
public:
    void isNewVersionAvailable();
    void setParameters(const QString& apiUrl, const QString& appName, const QString& downloadFileName);
private:
//	QString osName;
    QString apiUrl;
    QString appName;
    HttpWindow fetchTimes;
    QString downloadFileName;
    bool isParametersSet = false;
    QString apiPath = applicationDirPath + "/api.json";
    QString versionFileName = applicationDirPath + "/version.txt";
    bool compareTagVersion(const QString& tag, const QString& currentTag);
    QString openFile(const QString& fileName, QIODevice::OpenModeFlag flag=QIODevice::ReadOnly);
};

#endif // UPDATECONTROLLER_H
