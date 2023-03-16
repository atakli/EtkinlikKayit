#ifndef ACTIVITY_H
#define ACTIVITY_H

class Widget;
class QComboBox;
class QCompleter;
class QRadioButton;
class QStringListModel;

#include <QRadioButton>
#include <QSqlDatabase>
#include <QObject>
#include <QFile>

class Activity : public QObject
{
    Q_OBJECT
public:
	Activity(Widget* widget, QObject *parent = nullptr);
    ~Activity();
    void addToActivityListFile(QComboBox* comboBox);
	std::unique_ptr<QFile> openFile(const QString &fileName, QIODevice::OpenModeFlag omf);
    void addToParticipantListFile(QComboBox* comboBox, QList<QRadioButton*> categories);

	std::unique_ptr<QFile> activityListFile, participantListFile, logFile;
private:
	Widget* widget;
	QString punishedListFileNamePostFix = " Etkinliği Cezalı Listesi";
    QStringListModel* stringListModel;

    QSqlDatabase db;
	int createTable(const std::string& table);

    void addActivityParticipant(const QString& fileName, const QStringList& selectedParticipants);
    QStringList getLastThreeActivityDates(const QString& etkinlikFileName);
signals:
    void statusBarMessage(const QString& msg);
    void addItemToParticipantsWidget(const QString& msg);
    void startCompleter(QFile& file, QComboBox* comboBox);
public slots:
	void openPunishedList();
	void addActivity();
};

#endif // ACTIVITY_H
