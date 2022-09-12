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
class QComboBox;
class QStatusBar;
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    Ui::Widget *ui;
    QRadioButton* checkedButton;
	ParticipantsWidget *participantsWidget;
	void addToParticipantsWidget();
    const char* appName = "Etkinlik Kayıt Programı";
    QFile activityListFile, participantListFile, logFile;
    QStringListModel *modelFromFile(QFile &file);
    QCompleter* completer = nullptr;
    QStatusBar* statusBar;
    UpdateController update;
	void getParticipantsFromFile(QFile &file);
	QStringListModel* stringListModel;
	QStringList participantList;
    void addToFile(QFile &file, QComboBox *comboBox);
    void startCompleter(QFile &file, QComboBox *comboBox);
    void openFile(QFile& file, const QString &fileName, QIODevice::OpenModeFlag omf);
    void addActivity();
private slots:
    void highlightedIndex(int index);
    void highlightedString(const QString &text);
};
#endif // WIDGET_H
