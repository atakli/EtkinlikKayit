#ifndef PARTICIPANTSWIDGET_H
#define PARTICIPANTSWIDGET_H

#include <QWidget>

class WriteRegisterModel;

namespace Ui { class Form; }

class ParticipantsWidget : public QWidget
{
    Q_OBJECT
public:
    ParticipantsWidget(const QStringList& participantList, QWidget* parent = nullptr);
    void addItem(const QString& participant);
    void addItems(const QStringList& participants);
    void setParticipantList(const QStringList &newParticipantList);

private:
    int rowIndex = 0;
    Ui::Form *ui;
	QStringList participantList;
//    bool setHead
signals:
    void participantListUpdated();
};

#endif // PARTICIPANTSWIDGET_H
