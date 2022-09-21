#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>

#include "participantswidget.h"
#include "updatecontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
class QAbstractItemModel;
class QStringListModel;
class QRadioButton;
class QCompleter;
class QStatusBar;
class QComboBox;
class QMenuBar;
class QMenu;
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    Ui::Widget *ui;
    QStatusBar* statusBar;
    UpdateController update;
    QRadioButton* checkedButton;
    QCompleter* completer = nullptr;
	QStringListModel* stringListModel;
    ParticipantsWidget *participantsWidget;
    QStringList stringList, participantList;
    const char* appName = "Etkinlik Kayıt Programı";
    QFile activityListFile, participantListFile, logFile;

    void addActivity();
    void initializeMenusAndBars();
    void getFromFile(QFile &file);
    QStringListModel *modelFromFile(QFile &file);
    void addToFile(QFile &file, QComboBox *comboBox);
    void startCompleter(QFile &file, QComboBox *comboBox);
    QStringList getLastThreeActivityDates(const QString& etkinlikFileName);
    void openFile(QFile& file, const QString &fileName, QIODevice::OpenModeFlag omf);
    void addActivityParticipant(const QString& fileName, const QStringList& selectedParticipants);
private slots:
    void onTopAction();
    void highlightedIndex(int index);
    void highlightedString(const QString &text);
};
#endif // WIDGET_H
