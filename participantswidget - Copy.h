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
    //	void addItem();
    void setParticipantList(const QStringList &newParticipantList);

private:
    Ui::Form *ui;
	QStringList participantList;
	WriteRegisterModel *writeModel = nullptr;
signals:
    void participantListUpdated();
};

#endif // PARTICIPANTSWIDGET_H
