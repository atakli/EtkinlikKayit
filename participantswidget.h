#ifndef PARTICIPANTSWIDGET_H
#define PARTICIPANTSWIDGET_H

#include <QWidget>
namespace Ui { class Form; }

class ParticipantsWidget : public QWidget
{
    Q_OBJECT
public:
    ParticipantsWidget(QWidget* parent = nullptr);
private:
    Ui::Form *ui;
};

#endif // PARTICIPANTSWIDGET_H
