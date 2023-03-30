#ifndef PARTICIPANTSWIDGET_H
#define PARTICIPANTSWIDGET_H

#include <QWidget>

namespace Ui { class Form; }

#include "database.h"

class ParticipantsWidget : public QWidget
{
    Q_OBJECT
public:
    ParticipantsWidget(DataBase& db, QWidget* parent = nullptr);
    std::vector<int> getSelectedParticipants();
    void addItem(const Person& person);
private:
    int rowIndex = 0;
    Ui::Form *ui;
    QVector<Person> participantList;
signals:
    void participantListUpdated();
};

#endif // PARTICIPANTSWIDGET_H
