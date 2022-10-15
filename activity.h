#ifndef ACTIVITY_H
#define ACTIVITY_H

class QComboBox;
class QCompleter;
class QRadioButton;
class QStringListModel;

#include <QRadioButton>
#include <QObject>
#include <QFile>

class Activity : public QObject
{
    Q_OBJECT
public:
    Activity(QObject *parent = nullptr);
    void addToActivityListFile(QComboBox* comboBox);
    void openFile(QFile& file, const QString &fileName, QIODevice::OpenModeFlag omf);
    void addToParticipantListFile(QComboBox* comboBox, QList<QRadioButton*> categories);

    QFile activityListFile, participantListFile, logFile;
private:
    QStringListModel* stringListModel;

    QStringList getLastThreeActivityDates(const QString& etkinlikFileName);
signals:
    void statusBarMessage(const QString& msg);
    void addItemToParticipantsWidget(const QString& msg);
    void startCompleter(QFile& file, QComboBox* comboBox);
    void getYasCategories(QList<QRadioButton*>& categories);
};

#endif // ACTIVITY_H
