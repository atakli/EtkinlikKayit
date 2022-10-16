#include "activity.h"
#include "ui_widget.h"
#include "widget.h"

#include <QTextStream>
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
    QFile etkinlikKatilimcilariFile(etkinlikKatilimcilarilariFileName);
    openFile(etkinlikKatilimcilariFile, etkinlikKatilimcilarilariFileName + ".txt", QIODevice::ReadWrite);

    QStringList participants;
    etkinlikKatilimcilariFile.seek(0);
    while (!etkinlikKatilimcilariFile.atEnd())
    {
        QString line = etkinlikKatilimcilariFile.readLine();
        if (!line.isEmpty())
            participants << line.trimmed().split(",").at(0);
    }
    for (const auto& participant : widget->participantList)
    {
        auto iter = std::find(participants.begin(), participants.end(), participant);
        if (iter != participants.end())                 // bu etkinliğe kayıtlı
        {
            qDebug() << "1";
            auto iter1 = std::find(selectedParticipants.begin(), selectedParticipants.end(), *iter);
            if (iter1 == selectedParticipants.end())    // bu etkinliğe kayıtlı ama buna katılmamış
            {
                qDebug() << "3";
                int number = (*iter).split(",").last().toInt();
                if (number == 0)
                    continue;
                *iter = participant + "," + QString::number(--number);
                qDebug() << "burda 3:" << participants;
            }
            else                                        // bu etkinliğe kayıtlı ve buna da katılmış
            {
                qDebug() << "4";
                int number = (*iter).split(",").last().toInt();     // 3\n ifadesi toInt ile sayıya başarıyla dönüşür mü kontrol et
                if (number == 3)
                    continue;
                *iter = participant + "," + QString::number(++number);
                qDebug() << "burda 2:" << participants;
            }
        }
        else                                            // bu etkinliğe kayıtlı değil
        {
            qDebug() << "2";
            auto iter1 = std::find(selectedParticipants.begin(), selectedParticipants.end(), *iter);
            if (iter1 != selectedParticipants.end())    // bu etkinliğe kayıtlı değil, ilk defa katılıyor. else kısmı da ne kayıtlı ne de yeni katılanlar. ama tabiki yazacak bişey yok
            {
                participants << participant + ",3";
                qDebug() << "burda 1:" << participant;
            }
        }
    }
    etkinlikKatilimcilariFile.seek(0);                                  // TODO: sadece orayı değiştirmek diye bişey var mı? çünkü şuan baştan yazıyorum sanırım
    QTextStream stream(&etkinlikKatilimcilariFile);
    for (const auto& participant : participants)
        stream << participant + "\n";
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
		QMessageBox{}.warning(nullptr, appName, QString("Katılımcı seçmediniz!"));
		return;
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

void Activity::addToActivityListFile(QComboBox *comboBox)
{
    QTextStream stream(&activityListFile);

    if(comboBox->currentText().isEmpty())
    {
		QMessageBox{}.warning(nullptr, appName, "Etkinlik ismi girilmemiş!");
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
    }
}
QStringList Activity::getLastThreeActivityDates(const QString& etkinlikFileName)
{
    QFile file(etkinlikFileName);
    openFile(file, etkinlikFileName + ".txt", QIODevice::ReadWrite);
    // vektöre veya set'e atıp sıralayıp sonra son üçünü alabilirim. hatta son üçünü almak için tamamını sıralamama gerek olmayabilri. bunun bahsi geçmişti sanki
    return {};
}
