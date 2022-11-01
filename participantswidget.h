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
    std::vector<int> getSelectedParticipants();
private:
    int rowIndex = 0;
    Ui::Form *ui;
	QStringList* participantList;
signals:
    void participantListUpdated();
};

#endif // PARTICIPANTSWIDGET_H
