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
	void addItem();
private:
    Ui::Form *ui;
	QStringList participantList;
	WriteRegisterModel *writeModel = nullptr;
};

#endif // PARTICIPANTSWIDGET_H
