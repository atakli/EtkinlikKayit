#include "activity.h"
#include "ui_widget.h"
#include "widget.h"

#include <QTextStream>
#include <QListWidget>
#include <QMessageBox>
#include <QComboBox>

extern QString appName;

Activity::Activity(Widget* widget, QObject *parent) : QObject(parent), widget(widget)
{
    openFile(activityListFile, "etkinlikler.txt", QIODevice::ReadWrite);
    openFile(logFile, "logs.txt", QIODevice::ReadWrite);
    openFile(participantListFile, "katilimcilar.txt", QIODevice::ReadWrite);
}
void Activity::addActivityParticipant(const QString& fileName, const QStringList& selectedParticipants)
{
    QString etkinlikKatilimcilarilariFileName = fileName + "_Participants";
	QString participantsToBePunishedFileName = fileName + " Etkinliği Cezalı Listesi ";
    QFile etkinlikKatilimcilariFile(etkinlikKatilimcilarilariFileName);
	QFile participantsToBePunishedFile(participantsToBePunishedFileName);		// TODO: burda isim verip sonra tekrar isim atamak gibi gerksiz bi iş yapılıyo
    openFile(etkinlikKatilimcilariFile, etkinlikKatilimcilarilariFileName + ".txt", QIODevice::ReadWrite);
    openFile(participantsToBePunishedFile, participantsToBePunishedFileName + ".txt", QIODevice::ReadWrite);

    QStringList participants, participantsWithNumber, participantsToBePunished;
    etkinlikKatilimcilariFile.seek(0);
    while (!etkinlikKatilimcilariFile.atEnd())
    {
        QString line = etkinlikKatilimcilariFile.readLine();
        if (!line.isEmpty())
        {
            participantsWithNumber << line.trimmed();
            participants << line.trimmed().split(",").at(0);
        }
    }
    for (int i = 0; i < widget->participantList.size(); ++i)
    {
        QString participant = widget->participantList[i];
        auto iter = std::find(participants.begin(), participants.end(), participant);
        auto iter2 = std::find_if(participantsWithNumber.begin(), participantsWithNumber.end(), [&participant](const QString& str){return str.split(",").at(0) == participant;});
        if (iter != participants.end())                 // bu etkinliğe kayıtlı
        {
            auto iter1 = std::find(selectedParticipants.begin(), selectedParticipants.end(), *iter);
            int number = (*iter2).split(",").last().toInt();     // 3\n ifadesi toInt ile sayıya başarıyla dönüşür mü kontrol et
            if (iter1 == selectedParticipants.end())    // bu etkinliğe kayıtlı ama buna katılmamış
            {
                *iter = participant + "," + QString::number(number == 0 ? number : --number);
                if (number == 0)
                    participantsToBePunished << participant;
            }
            else                                        // bu etkinliğe kayıtlı ve buna da katılmış
            {
                *iter = participant + "," + QString::number(number == 3 ? number : ++number);
            }
        }
        else                                            // bu etkinliğe kayıtlı değil
        {
            auto iter1 = std::find(selectedParticipants.begin(), selectedParticipants.end(), participant);
            if (iter1 != selectedParticipants.end())    // bu etkinliğe kayıtlı değil, ilk defa katılıyor. else kısmı da ne kayıtlı ne de yeni katılanlar. ama tabiki yazacak bişey yok
            {
                participants << participant + ",3";
            }
        }
    }
    etkinlikKatilimcilariFile.seek(0);                                  // TODO: sadece orayı değiştirmek diye bişey var mı? çünkü şuan baştan yazıyorum sanırım
    QTextStream stream(&etkinlikKatilimcilariFile);
    QTextStream stream1(&participantsToBePunishedFile);
    for (const auto& participant : participants)
        stream << participant + "\n";
    for (const auto& participant : participantsToBePunished)
        stream1 << participant + "\n";
    stream.flush();
}
void Activity::addActivity()
{
	QString etkinlikFileName = widget->ui->etkinlikComboBox->currentText();

    activityListFile.seek(0);
    QStringList activities = QString(activityListFile.readAll()).split("\n");

    if (etkinlikFileName.isEmpty() || !activities.contains(etkinlikFileName))
	{
        QMessageBox{}.warning(nullptr, appName, QString("Etkinlik ismi girmediniz veya yanlış girdiniz!"));
		return;
	}
	QString date = widget->ui->dateEdit->text();	// TODO: bunun da düzgün bi tarih olup olmadığını kontrol edeyim
	QFile etkinlikFile(etkinlikFileName);

	openFile(etkinlikFile, etkinlikFileName + ".txt", QIODevice::ReadWrite);

	etkinlikFile.seek(etkinlikFile.size());
	QTextStream stream(&etkinlikFile);
	auto selectedParticipants = widget->participantsWidget->getSelectedParticipants();
	QStringList participants;
	if (selectedParticipants.empty())
	{
		if (QMessageBox(QMessageBox::Question, appName, "Katılımcı seçmediniz! Kimse Gelmedi mi?", QMessageBox::Retry | QMessageBox::Apply).exec() == QMessageBox::Retry)
		{
			return;
		}
//		QMessageBox{}.warning(nullptr, appName, QString("Katılımcı seçmediniz!"));
//		return;
	}
	else
		for (auto index : selectedParticipants)
		{
			stream << date << ", " << widget->participantList.at(index) << "\n";
			participants << widget->participantList.at(index);
		}

	addActivityParticipant(etkinlikFileName, participants);
	stream.flush();
    emit statusBarMessage(QString("Seçili kişi(ler) \"%1\" etkinliğine kaydedildi").arg(etkinlikFileName));
}
void Activity::addToParticipantListFile(QComboBox* comboBox, QList<QRadioButton*> categories)
{
    QTextStream stream(&participantListFile);

    auto checkedButtonIter = std::find_if(std::cbegin(categories), std::cend(categories), [](const auto&button){return button->isChecked();});
    /*Warns when a lambda inside a connect() captures local variables by reference.
    Example:
    int a;
    connect(obj, &MyObj::mySignal, [&a] { ... });
    This usually results in a crash since the lambda might get called after the captured variable went out of scope.*/
    QRadioButton* checkedButton = *checkedButtonIter;
    if(comboBox->currentText().isEmpty())
    {
		QMessageBox{}.warning(nullptr, appName, "Kişi ismi girilmemiş!");
        return;
    }
    else if (checkedButtonIter == categories.cend())
    {
		QMessageBox{}.warning(nullptr, appName, QString("%1 için yaş kategorisi seçilmemiş!").arg(comboBox->currentText()));
        return;
    }

	emit statusBarMessage(QString("\"%1\" kategorisindeki \"%2\" kaydedildi").arg(checkedButton->text(), comboBox->currentText()));
    stream << comboBox->currentText() << " (" << checkedButton->text() << ")\n";

    checkedButton->setAutoExclusive(false);
    checkedButton->setChecked(false);
    checkedButton->setAutoExclusive(true);

    emit addItemToParticipantsWidget(comboBox->currentText() + " (" + checkedButton->text() + ")");

    stream.flush();
    emit startCompleter(participantListFile, comboBox);

	comboBox->clearEditText();
}

void Activity::openPunishedList()
{
	activityListFile.seek(0);
	QStringList activities = QString(activityListFile.readAll()).split("\n");

	QString activityName = widget->ui->etkinlikComboBox->currentText();
	if (activityName.isEmpty() || !activities.contains(activityName))
	{
		QMessageBox::warning(nullptr, appName, QString("Etkinlik ismi girmediniz veya yanlış girdiniz!"));
		return;
	}
	QString participantsToBePunishedFileName = activityName + " Etkinliği Cezalı Listesi ";
	QFile participantsToBePunishedFile(participantsToBePunishedFileName);
	openFile(participantsToBePunishedFile, participantsToBePunishedFileName + ".txt", QIODevice::ReadWrite);
	participantsToBePunishedFile.seek(0);
	QStringList participantsToBePunished;
	while (!participantsToBePunishedFile.atEnd())
	{
		QString line = participantsToBePunishedFile.readLine();
		if (!line.isEmpty())
		{
			participantsToBePunished << line.trimmed();
		}
	}
	if (participantsToBePunished.isEmpty())
	{
		QMessageBox::information(nullptr, appName, QString("Cezalı kimse yok"));
		return;
	}

	qDebug() << participantsToBePunished;

	QWidget *layoutWidget = new QWidget();
	layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
	layoutWidget->setWindowTitle(activityName);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
//	sizePolicy.setHeightForWidth(adSoyadComboBox->sizePolicy().hasHeightForWidth());
	layoutWidget->setSizePolicy(sizePolicy);
//	layoutWidget->setMinimumSize(QSize(460, 1000));
//	layoutWidget->resize(layoutWidget->minimumSizeHint());
//	layoutWidget->adjustSize();

	QListWidget *listWidget = new QListWidget(layoutWidget);
	for (const QString& participant : participantsToBePunished)
	{
		new QListWidgetItem(participant, listWidget);
	}

	QVBoxLayout *verticalLayout = new QVBoxLayout();
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	verticalLayout->addWidget(listWidget);
	layoutWidget->show();
}

void Activity::addToActivityListFile(QComboBox *comboBox)
{
    QTextStream stream(&activityListFile);
    activityListFile.seek(0);
    QStringList activities = QString(activityListFile.readAll()).split("\n");

    if(comboBox->currentText().isEmpty() || activities.contains(comboBox->currentText()))
    {
        QMessageBox{}.warning(nullptr, appName, "Etkinlik ismi girilmemiş veya bu etkinlik zaten var!");
        return;
    }
    emit statusBarMessage(QString("\"%1\" kaydedildi").arg(comboBox->currentText()));
    stream << comboBox->currentText() << '\n';

    stream.flush();
    emit startCompleter(activityListFile, comboBox);

    comboBox->clearEditText();
}
void Activity::openFile(QFile& file, const QString& fileName, QIODevice::OpenModeFlag flag)       // error return ekle veya exception
{
    file.setFileName(fileName);

    if (!file.open(flag))
    {
        file.close();
		QMessageBox{}.critical(nullptr, appName, QString("%1 dosyası açılamadı!").arg(fileName));
		exit(EXIT_FAILURE);
    }
}
QStringList Activity::getLastThreeActivityDates(const QString& etkinlikFileName)
{
    QFile file(etkinlikFileName);
    openFile(file, etkinlikFileName + ".txt", QIODevice::ReadWrite);
    // vektöre veya set'e atıp sıralayıp sonra son üçünü alabilirim. hatta son üçünü almak için tamamını sıralamama gerek olmayabilri. bunun bahsi geçmişti sanki
    return {};
}
