#include "participantswidget.h"
#include "qcheckbox.h"
#include "ui_participantlist.h"
#include "writeregistermodel.h"

ParticipantsWidget::ParticipantsWidget(const QStringList& participantList, QWidget* parent) : QWidget(parent), ui(new Ui::Form), participantList(participantList)
{
    ui->setupUi(this);
    addItems(participantList);
}

void ParticipantsWidget::setParticipantList(const QStringList &newParticipantList)
{
    participantList = newParticipantList;
}

void ParticipantsWidget::addItem(const QString& participant)
{
    /*class HeaderView : public QAbstractItemModel
    {
    public:
        bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override
        {
            headerDataChanged(orientation, );
            return true;
        }
    };

    QCheckBox *cellCheckBox = qobject_cast<QCheckBox *>(ui->tableWidget->cellWidget(0, 0));
    QHeaderView* headerView = new QHeaderView(Qt::Horizontal,  ui->tableWidget);
    headerView->set
    ui->tableWidget->insertRow(0);
    QTableWidgetItem *cubesHeaderItem = new QTableWidgetItem(tr("Cubes"));
    QTableWidgetItem *cubesHeaderItem1 = new QTableWidgetItem(checkBox);
    ui->tableWidget->setHorizontalHeader(headerView);
    QHeaderView* headerView1 = ui->tableWidget->horizontalHeader();
    ui->tableWidget->setColumnWidth(0, ui->tableWidget->columnWidth(1) / 3);
    QTableWidgetItem* firstColumnHeaderItem = ui->tableWidget->horizontalHeaderItem(0);
    QCheckBox *cellCheckBox = static_cast<QCheckBox *>(firstColumnHeaderItem);*/

    participantList.append(participant);

    QString participantName = participant.split('(')[0].chopped(1);
    QString category = participant.mid(participantName.size() + 2).chopped(1);

    ui->tableWidget->insertRow(rowIndex);

    ui->tableWidget->setCellWidget(rowIndex, 0, new QCheckBox);
    ui->tableWidget->setItem(rowIndex, 1, new QTableWidgetItem(participantName));
    ui->tableWidget->setItem(rowIndex, 2, new QTableWidgetItem(category));
    ui->tableWidget->resizeColumnsToContents();
    ++rowIndex;
}

void ParticipantsWidget::addItems(const QStringList &participants)
{
    for (; rowIndex < participants.size(); ++rowIndex)
    {
        QString participantName = participants.at(rowIndex).split('(')[0].chopped(1);
        QString category = participants.at(rowIndex).mid(participantName.size() + 2).chopped(1);

        ui->tableWidget->insertRow(rowIndex);

        ui->tableWidget->setCellWidget(rowIndex, 0, new QCheckBox);
        ui->tableWidget->setItem(rowIndex, 1, new QTableWidgetItem(participantName));
        ui->tableWidget->setItem(rowIndex, 2, new QTableWidgetItem(category));
        ui->tableWidget->resizeColumnsToContents();
    }
}
