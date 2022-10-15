#include "activity.h"

#include <QTextStream>
#include <QMessageBox>
#include <QComboBox>

extern QString appName;

Activity::Activity(QObject *parent) : QObject(parent)
{
    openFile(activityListFile, "etkinlikler.txt", QIODevice::ReadWrite);
    openFile(logFile, "logs.txt", QIODevice::ReadWrite);
    openFile(participantListFile, "katilimcilar.txt", QIODevice::ReadWrite);
}

void Activity::addToParticipantListFile(QComboBox* comboBox, QList<QRadioButton*> categories)
{
    QTextStream stream(&participantListFile);
    QMessageBox qmbox;

//    auto categories = getYasCategories();
//    emit getYasCategories(categories);
    auto checkedButtonIter = std::find_if(std::cbegin(categories), std::cend(categories), [](const auto&button){return button->isChecked();});
    /*Warns when a lambda inside a connect() captures local variables by reference.
    Example:
    int a;
    connect(obj, &MyObj::mySignal, [&a] { ... });
    This usually results in a crash since the lambda might get called after the captured variable went out of scope.*/
    QRadioButton* checkedButton = *checkedButtonIter;
    if(comboBox->currentText().isEmpty())
    {
        qmbox.warning(nullptr, appName, "Kişi ismi girilmemiş!");
        return;
    }
    else if (checkedButtonIter == categories.cend())
    {
        qmbox.warning(nullptr, appName, QString("%1 için yaş kategorisi seçilmemiş!").arg(comboBox->currentText()));
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
    QMessageBox qmbox;

    if(comboBox->currentText().isEmpty())
    {
        qmbox.warning(nullptr, appName, "Etkinlik ismi girilmemiş!");
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
        QMessageBox qmbox;
        qmbox.critical(nullptr, appName, QString("%1 dosyası açılamadı!").arg(fileName));
    }
}
QStringList Activity::getLastThreeActivityDates(const QString& etkinlikFileName)
{
    QFile file(etkinlikFileName);
    openFile(file, etkinlikFileName + ".txt", QIODevice::ReadWrite);
    // vektöre veya set'e atıp sıralayıp sonra son üçünü alabilirim. hatta son üçünü almak için tamamını sıralamama gerek olmayabilri. bunun bahsi geçmişti sanki
    return {};
}
