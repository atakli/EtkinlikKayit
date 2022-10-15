#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>

#include "participantswidget.h"
#include "updatecontroller.h"
#include "activity.h"

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
    QCompleter* completer = nullptr;
    std::unique_ptr<Activity> activity;
    ParticipantsWidget *participantsWidget;
    QStringList participantList;
//    const char* appName = "Etkinlik Kayıt Programı";

    void addActivity();
    void initializeMenusAndBars();
    QStringListModel *modelFromFile(QFile &file);
    void addActivityParticipant(const QString& fileName, const QStringList& selectedParticipants);
    std::pair<QStringList, QStringListModel*> getFromFile(QFile &file, QStringList& participantList);
private slots:
    void onTopAction();
    void highlightedIndex(int index);
    void highlightedString(const QString &text);
    void startCompleter(QFile &file, QComboBox *comboBox);
};
#endif // WIDGET_H
