#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>

#include "updatecontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
class QAbstractItemModel;
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
    const char* appName = "Etkinlik Kayıt Programı";
    QFile activityListFile, participantListFile, logFile;
    QAbstractItemModel *modelFromFile(QFile &file);
    QCompleter* completer = nullptr;
    QStatusBar* statusBar;
    UpdateController update;
    void addToFile(QFile &file, QComboBox *comboBox);
    void startCompleter(QFile &file, QComboBox *comboBox);
    void openFile(QFile& file, const char* fileName, QIODevice::OpenModeFlag omf);
private slots:
    void highlightedIndex(int index);
    void highlightedString(const QString &text);
};
#endif // WIDGET_H
