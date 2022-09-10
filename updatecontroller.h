#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include "fetchtimes.h"

#include <QWidget>
#include <QObject>
#include <QDialog>

class UpdateController : public QObject
{
    Q_OBJECT
public:
	void isNewVersionAvailable();

    void setApiUrl(const QString &newApiUrl);
    void setVersionFileName(const QString &newVersionFileName);
    void setAppName(const QString &newAppName);
    void setApiPath(const QString &newApiPath);
    void setDownloadFileName(const QString &newDownloadFileName);
private:
    QString openFile(const QString& fileName, QIODevice::OpenModeFlag flag=QIODevice::ReadOnly);
//	QString osName;
    QString apiUrl;
    QString apiPath;
    QString appName;
    QString downloadFileName;
    QString versionFileName;
	HttpWindow fetchTimes;
	bool compareTagVersion(const QString& tag, QString currentTag);
};

#endif // UPDATECONTROLLER_H
