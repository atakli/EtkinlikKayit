#ifndef PARTICIPANTSWIDGET_H
#define PARTICIPANTSWIDGET_H

#include <QWidget>

namespace Ui { class Form; }

class ParticipantsWidget : public QWidget
{
    Q_OBJECT
public:
    ParticipantsWidget(QStringList* participantList, QWidget* parent = nullptr);
    void addItem(const QString& participant);
    void addItems();
private:
    int rowIndex = 0;
    Ui::Form *ui;
    QStringList* participantList;
private slots:
    void selectAllCheckBoxes(bool state);
signals:
    void participantListUpdated();
};

#endif // PARTICIPANTSWIDGET_H
