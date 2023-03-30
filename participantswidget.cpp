#include "participantswidget.h"
#include "ui_participantlist.h"
#include "qcheckbox.h"

#include <QDebug>

//ParticipantsWidget::ParticipantsWidget(QStringList* participantList, QWidget* parent) : QWidget(parent), ui(new Ui::Form), participantList(participantList)
ParticipantsWidget::ParticipantsWidget(DataBase& db, QWidget* parent) : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);
//	std::for_each(participantList->cbegin(), participantList->cend(), [this](const QString& participant){addItem(participant);});

    QSqlQuery query("SELECT * from katilimcilar", db.getDataBase());
    while (query.next())
    {
        const Person person = {query.value(0).toString(), query.value(1).toString(), query.value(2).toString()};
        participantList.append(person);
        addItem(person);
    }

	static auto selectAllCheckBoxes = [this](bool state)
	{
        for (int i = 0; i < this->participantList.size(); ++i)
			static_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 0))->setChecked(state);
	};

	connect(ui->tumunuSecPushButton, &QPushButton::clicked, [&](){selectAllCheckBoxes(true);});
	connect(ui->hicbiriniSecmePushButton, &QPushButton::clicked, [&](){selectAllCheckBoxes(false);});
}

void ParticipantsWidget::addItem(const Person& person)
{
//    QString participantName = participant.split('(')[0].chopped(1);
//    QString category = participant.mid(participantName.size() + 2).chopped(1);

    ui->tableWidget->insertRow(rowIndex);

    ui->tableWidget->setCellWidget(rowIndex, 0, new QCheckBox);
    ui->tableWidget->setItem(rowIndex, 1, new QTableWidgetItem(person.name));
    ui->tableWidget->setItem(rowIndex, 2, new QTableWidgetItem(person.surname));
    ui->tableWidget->setItem(rowIndex, 3, new QTableWidgetItem(person.group));
    ui->tableWidget->resizeColumnsToContents();
    ++rowIndex;
}

std::vector<int> ParticipantsWidget::getSelectedParticipants()
{
    std::vector<int> selectedParticipants;
    for (int i = 0; i < participantList.size(); ++i)   // participantList olusturmak gereksiz sanki. direk widgettan eleman sayisini alabilmeliyim
        if (static_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 0))->isChecked())
            selectedParticipants.push_back(i);
    return selectedParticipants;
}

//void ParticipantsWidget::updateWidget(const DataBase &db)
//{
//    QSqlQuery query("SELECT * from katilimcilar", db);
//    while (query.next())
//    {

//    }
//}
