#include "participantswidget.h"
#include "ui_participantlist.h"
#include "writeregistermodel.h"

ParticipantsWidget::ParticipantsWidget(const QStringList& participantList, QWidget* parent) : QWidget(parent), ui(new Ui::Form), participantList(participantList)
{
    ui->setupUi(this);

	writeModel = new WriteRegisterModel(participantList, this);
	writeModel->setStartAddress(0);
	writeModel->setNumberOfValues(QString::number(participantList.size()));

	ui->treeView->setModel(writeModel);
	ui->treeView->hideColumn(2);
//	connect(writeModel, &WriteRegisterModel::updateViewport, ui->treeView->viewport(), QOverload<>::of(&QWidget::update));
    connect(this, &ParticipantsWidget::participantListUpdated, writeModel, &WriteRegisterModel::updateParticipantList);
}

void ParticipantsWidget::setParticipantList(const QStringList &newParticipantList)
{
    participantList = newParticipantList;
    writeModel->setParticipantList(participantList);
}

//void ParticipantsWidget::addItem()
//{
//	new QListWidgetItem("asdaf", ui->listWidget);
	//	ui->treeView->addItem("adc");
//}
