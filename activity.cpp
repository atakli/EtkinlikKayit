#include "ui_widget.h"
#include "activity.h"
#include "widget.h"

#include <QTextStream>
#include <QListWidget>
#include <QMessageBox>
#include <QComboBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

extern QString appName;

Activity::Activity(Widget* widget, QObject *parent) : QObject(parent),
	activityListFile(openFile("etkinlikler.txt", QIODevice::ReadWrite)),
	participantListFile(openFile("katilimcilar.txt", QIODevice::ReadWrite)),
	logFile(openFile("logs.txt", QIODevice::ReadWrite)),
    widget(widget)
{
//    db.createTable("katilimcilar (id INTEGER PRIMARY KEY, isim TEXT, soyisim TEXT, grup TEXT)");
    db.createTable("katilimcilar (isim TEXT, soyisim TEXT, grup TEXT)");
//    db.insertValue("katilimcilar", {"'1emre'", "'6ataklı'", "'7genç'"});    // column name'leri optional imis
//    db.insertValue("katilimcilar", "(isim, soyisim, grup) VALUES ('2emre', '5ataklı', '8genç')");    // column name'leri optional imis
//    qDebug() << "id:" << db.calculate_current_id();
//    QSqlQuery query(db.getDataBase());

//    if (!query.last()) {
//        qDebug() << "Failed to move to last record!";
//        return;
//    }
    int cellCount = 0;
    const auto id = db.calculate_current_id_of_table("katilimcilar");
    if (id != RowIdQueryFailed && id != NoEntry)
        cellCount = id;

//    db.createTable("etkinlikler");
}

void Activity::addActivityParticipant(const QString& fileName, const QStringList& selectedParticipants)
{
	auto etkinlikKatilimcilariFile = openFile(fileName + "_Participants.txt", QIODevice::ReadWrite);
	auto participantsToBePunishedFile = openFile(fileName + punishedListFileNamePostFix + ".txt", QIODevice::ReadWrite);

    QStringList participants, participantsWithNumber, participantsToBePunished;
	etkinlikKatilimcilariFile->seek(0);
	while (!etkinlikKatilimcilariFile->atEnd())
    {
		QString line = etkinlikKatilimcilariFile->readLine();
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
				*iter = participant + "," + QString::number(number == 0 ? number : --number) + '\n';
                if (number == 0)
                    participantsToBePunished << participant;
            }
            else                                        // bu etkinliğe kayıtlı ve buna da katılmış
            {
				*iter = participant + "," + QString::number(number == 3 ? number : ++number) + '\n';
            }
        }
        else                                            // bu etkinliğe kayıtlı değil
        {
            auto iter1 = std::find(selectedParticipants.begin(), selectedParticipants.end(), participant);
            if (iter1 != selectedParticipants.end())    // bu etkinliğe kayıtlı değil, ilk defa katılıyor. else kısmı da ne kayıtlı ne de yeni katılanlar. ama tabiki yazacak bişey yok
            {
				participants << participant + ",3\n";
            }
        }
    }
	etkinlikKatilimcilariFile->seek(0);                                  // TODO: sadece orayı değiştirmek diye bişey var mı? çünkü şuan baştan yazıyorum sanırım
	QTextStream stream(etkinlikKatilimcilariFile.get());
	QTextStream stream1(participantsToBePunishedFile.get());

	std::for_each(participants.cbegin(), participants.cend(), [&stream](const auto&str){stream << str;});
	std::for_each(participantsToBePunished.cbegin(), participantsToBePunished.cend(), [&stream1](const auto&str){stream1 << str;});

    stream.flush();
}
void Activity::addActivity()
{
	const QString etkinlikFileName = widget->ui->etkinlikComboBox->currentText();
	if (etkinlikFileName.isEmpty())
	{
		QMessageBox::warning(nullptr, appName, QString("Etkinlik ismi girmediniz!"));
		return;
	}

	activityListFile->seek(0);
	if (!QString(activityListFile->readAll()).split("\n").contains(etkinlikFileName))
	{
		QMessageBox::warning(nullptr, appName, QString("Etkinlik ismini yanlış girdiniz!"));
		return;
	}
	const auto selectedParticipants = widget->participantsWidget->getSelectedParticipants();
	if (selectedParticipants.empty())
	{
		QMessageBox::warning(nullptr, appName, QString("Katılımcı seçmediniz!")); // TODO: kimse gelmedi butonu ekle
	//	if (QMessageBox(QMessageBox::Question, appName, "Katılımcı seçmediniz! Kimse Gelmedi mi?", QMessageBox::Retry | QMessageBox::Apply).exec() == QMessageBox::Retry)
		return;	// TODO: bu yokmus. unutmus muyum, yoksa bilerek mi bilmiyorum
	}
	auto etkinlikFile = openFile(etkinlikFileName + ".txt", QIODevice::ReadWrite);

	etkinlikFile->seek(etkinlikFile->size());
	QTextStream stream(etkinlikFile.get());
	const QStringList participants = [&]
	{
		const QString date = widget->ui->dateEdit->text();	// TODO: bunun da düzgün bi tarih olup olmadığını kontrol edeyim
		QStringList participants;
		for (const auto index : selectedParticipants)
		{
			const auto participant = widget->participantList.at(index);
			stream << date << ", " << participant << "\n";
			participants << participant;
		}
		return participants;
	}();
	addActivityParticipant(etkinlikFileName, participants);
	stream.flush();
    emit statusBarMessage(QString("Seçili kişi(ler) \"%1\" etkinliğine kaydedildi").arg(etkinlikFileName));
}
void Activity::addToParticipantListFile(QComboBox* comboBox, QComboBox* soyadComboBox, QList<QRadioButton*> categories)
{
	QTextStream stream(participantListFile.get());

	const auto checkedButtonIter = std::find_if(std::cbegin(categories), std::cend(categories), [](const auto&button){return button->isChecked();});

    if(comboBox->currentText().isEmpty() || soyadComboBox->currentText().isEmpty())
    {
        QMessageBox::warning(nullptr, appName, "Kişi ismi veya soyismi girilmemiş!");
        return;
    }
    if (checkedButtonIter == categories.cend())
    {
		QMessageBox::warning(nullptr, appName, QString("%1 için yaş kategorisi seçilmemiş!").arg(comboBox->currentText()));
        return;
    }
    QRadioButton* checkedButton = *checkedButtonIter;
    const Person person = {comboBox->currentText(), soyadComboBox->currentText(), checkedButton->text()};

    emit statusBarMessage(QString("\"%1\" kategorisindeki \"%2\" \"%3\" kaydedildi").arg(person.group, person.name, person.surname));
    stream << comboBox->currentText() << " (" << person.group << ")\n";

    auto f = [](QString str){str.push_back('\''); str.push_front('\''); return str;};  //100540.99

    db.insertValue("katilimcilar", {f(person.group), f(person.name), f(person.surname)});

    checkedButton->setAutoExclusive(false);
    checkedButton->setChecked(false);
    checkedButton->setAutoExclusive(true);

    emit addItemToParticipantsWidget(person);

    stream.flush();
	emit startCompleter(*participantListFile, comboBox);

    comboBox->clearEditText();
}

DataBase &Activity::getDb()
{
    return db;
}

void Activity::openPunishedList()
{
	const QString activityName = widget->ui->etkinlikComboBox->currentText();
	if (activityName.isEmpty())
	{
		QMessageBox::warning(nullptr, appName, QString("Etkinlik ismi girmediniz!"));
		return;
	}
	activityListFile->seek(0);
	if (!QString(activityListFile->readAll()).split("\n").contains(activityName))
	{
		QMessageBox::warning(nullptr, appName, QString("Etkinlik ismini yanlış girdiniz!"));
		return;
	}
	const QString participantsToBePunishedFileName = activityName + punishedListFileNamePostFix;
	auto participantsToBePunishedFile = openFile(participantsToBePunishedFileName + ".txt", QIODevice::ReadWrite);
	participantsToBePunishedFile->seek(0);
	const QStringList participantsToBePunished = [&]
	{
		QStringList participantsToBePunished;
		while (!participantsToBePunishedFile->atEnd())
		{
			const QString line = participantsToBePunishedFile->readLine();
			if (!line.isEmpty())
			{
				participantsToBePunished << line.trimmed();
			}
		}
		return participantsToBePunished;
	}();
	if (participantsToBePunished.isEmpty())
	{
		QMessageBox::information(nullptr, appName, QString("Cezalı kimse yok"));
		return;
	}

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
	const QString activityName = comboBox->currentText();
	if(activityName.isEmpty())
    {
		QMessageBox::warning(nullptr, appName, "Etkinlik ismi girilmemiş!");
        return;
    }
	activityListFile->seek(0);
	if(QString(activityListFile->readAll()).split("\n").contains(activityName))
	{
		QMessageBox::warning(nullptr, appName, "Bu etkinlik zaten var!");
		return;
	}
	QTextStream stream(activityListFile.get());
	stream << activityName << '\n';
    stream.flush();
	emit startCompleter(*activityListFile, comboBox);
	emit statusBarMessage(QString("\"%1\" kaydedildi").arg(activityName));

    comboBox->clearEditText();
}
std::unique_ptr<QFile> Activity::openFile(const QString& fileName, QIODevice::OpenModeFlag flag)       // error return ekle veya exception
{
	auto file = std::make_unique<QFile>(fileName);
	if (!file->open(flag))
	{
		file->close();
		QMessageBox::critical(nullptr, appName, QString("%1 dosyası açılamadı!").arg(fileName));
		exit(EXIT_FAILURE);
	}
	return file;
}
QStringList Activity::getLastThreeActivityDates(const QString& etkinlikFileName)
{
	auto file = openFile(etkinlikFileName + ".txt", QIODevice::ReadWrite);
    // vektöre veya set'e atıp sıralayıp sonra son üçünü alabilirim. hatta son üçünü almak için tamamını sıralamama gerek olmayabilri. bunun bahsi geçmişti sanki
    return {};
}
