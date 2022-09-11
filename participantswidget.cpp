#include "participantswidget.h"
#include "ui_participantlist.h"

ParticipantsWidget::ParticipantsWidget(QWidget* parent) : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);
    ui->listWidget->addItem("adc");
}
