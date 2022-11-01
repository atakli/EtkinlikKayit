#include "participantswidget.h"
#include "ui_participantlist.h"
#include "qcheckbox.h"

#include <QDebug>

ParticipantsWidget::ParticipantsWidget(QStringList* participantList, QWidget* parent) : QWidget(parent), ui(new Ui::Form), participantList(participantList)
{
    ui->setupUi(this);
	std::for_each(participantList->cbegin(), participantList->cend(), [this](const QString& participant){addItem(participant);});

	static auto selectAllCheckBoxes = [this](bool state)
	{
		for (int i = 0; i < this->participantList->size(); ++i)
			static_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 0))->setChecked(state);
	};

	connect(ui->tumunuSecPushButton, &QPushButton::clicked, [&](){selectAllCheckBoxes(true);});
	connect(ui->hicbiriniSecmePushButton, &QPushButton::clicked, [&](){selectAllCheckBoxes(false);});
}

void ParticipantsWidget::addItem(const QString& participant)
{
    QString participantName = participant.split('(')[0].chopped(1);
    QString category = participant.mid(participantName.size() + 2).chopped(1);

    ui->tableWidget->insertRow(rowIndex);

    ui->tableWidget->setCellWidget(rowIndex, 0, new QCheckBox);
    ui->tableWidget->setItem(rowIndex, 1, new QTableWidgetItem(participantName));
    ui->tableWidget->setItem(rowIndex, 2, new QTableWidgetItem(category));
    ui->tableWidget->resizeColumnsToContents();
    ++rowIndex;
}

std::vector<int> ParticipantsWidget::getSelectedParticipants()
{
    std::vector<int> selectedParticipants;
    for (int i = 0; i < participantList->size(); ++i)
        if (static_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 0))->isChecked())
            selectedParticipants.push_back(i);
    return selectedParticipants;
}
