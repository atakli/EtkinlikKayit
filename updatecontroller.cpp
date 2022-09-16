#include "updatecontroller.h"
//#include "calcTimes.h"

#include <QJsonDocument>
#include <QMessageBox>
#include <QFile>

void UpdateController::setParameters(const QString &apiUrl, const QString &appName, const QString &downloadFileName)
{
    this->apiUrl = apiUrl;
    this->appName = appName;
    this->downloadFileName = downloadFileName;
    isParametersSet = true;
}

QString UpdateController::openFile(const QString& fileName, QIODevice::OpenModeFlag flag)
{
	QFile file(fileName);
    if (!file.open(flag | QIODevice::Text))
		return "";							// TODO: buraya girerse ne olcak?
    return file.readAll();
}
bool UpdateController::compareTagVersion(const QString& tag, const QString& currentTag) // TODO: .'lar arasında birden fazla basamak olursa da çalışmalı
{
//	osName = currentTag.split(".").last().split("-").last().trimmed();
    QStringList tagList = tag.mid(1).split("-").at(0).split(".");
    QStringList currentTagList = currentTag.mid(1).split("-").at(0).split(".");
    for (int i = 0; i < 3; ++i)
        if (tagList.at(i).toInt() > currentTagList.at(i).toInt())
            return true;
    return false;
}
void UpdateController::isNewVersionAvailable()
{
    if (!isParametersSet)
    {
        QMessageBox qmbox;
        qmbox.warning(nullptr, tr(appName.toStdString().c_str()), "Güncelleme Kontrolcüsüne parametreler geçilmemiş\nGüncelleme olup olmadığını kontrol edebilmek için gerekli parametreleri geçip tekrar deneyin");
        return;
    }
    fetchTimes.downloadSynchronous(apiPath, apiUrl, "");

    const QString saveData = openFile(apiPath);
//	QJsonDocument loadDoc = QJsonDocument::fromVariant(saveData);
    const QJsonDocument loadDoc = QJsonDocument::fromJson(QByteArray::fromStdString(saveData.toStdString()));

    const QString currentTag = openFile(versionFileName);
    if(const QString tag = loadDoc["tag_name"].toString(); compareTagVersion(tag, currentTag))
    {
        if (QMessageBox(QMessageBox::Question, appName, "Yeni sürüm bulundu\nİndirilelim mi?", QMessageBox::No | QMessageBox::Yes).exec() == QMessageBox::No)
		{
			return;
		}
//            https://github.com/atakli/EtkinlikKayit/releases/latest/download/EtkinlikKayit.zip
        fetchTimes.downloadSynchronous("", loadDoc["assets"][0]["browser_download_url"].toString(), downloadFileName); // ismi PrayerReminder.zip'dan başka bişey olursa diye // TODO: 0'da sıkıntı olabilir
//		fetchTimes.downloadSynchronous("", "https://github.com/atakli/PrayerReminder-Desktop/releases/latest/download/PrayerReminder-" + osName + ".zip");
//        fetchTimes.downloadSynchronous("", "https://github.com/atakli/PrayerReminder-Desktop/releases/latest/download/PrayerReminder" + QString(".zip"));
	}
	else
	{
		QMessageBox qmbox;
        qmbox.information(nullptr, tr(appName.toStdString().c_str()), "Program güncel");
    }
}
