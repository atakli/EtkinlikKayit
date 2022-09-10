#include "widget.h"
#include "qlineedit.h"
#include "ui_widget.h"

#include <algorithm>

#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QStringListModel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QCompleter>
#include <QStatusBar>
#include <QListView>
#include <QDebug>

extern QString applicationDirPath;

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    openFile(activityListFile, "etkinlikler.txt", QIODevice::ReadWrite);
    openFile(logFile, "logs.txt", QIODevice::ReadWrite);
    openFile(participantListFile, "katilimcilar.txt", QIODevice::ReadWrite);

    update.setApiUrl("https://api.github.com/repos/atakli/PrayerReminder-Desktop/releases/latest"); // TODO change it!
    update.setVersionFileName(applicationDirPath + "/version.txt");
    update.setAppName(appName);
    update.setApiPath(applicationDirPath + "/api.json");
    update.setDownloadFileName("etkinlikKayit.zip");
    update.isNewVersionAvailable();

    connect(ui->etkinlikEklePushButton, &QPushButton::clicked, this, [this](){this->addToFile(activityListFile, ui->etkinlikComboBox);});
    connect(ui->kisiEklePushButton, &QPushButton::clicked, this, [this](){this->addToFile(participantListFile, ui->adSoyadComboBox);});

    connect(ui->adSoyadComboBox, &QComboBox::highlighted, this, &Widget::highlightedIndex);
    connect(ui->adSoyadComboBox, &QComboBox::textHighlighted, this, &Widget::highlightedString);

    ui->dateEdit->setDate(QDateTime::currentDateTime().date());

    statusBar = new QStatusBar(this);
    ui->horizontalLayoutBottom->addWidget(statusBar);

    ui->etkinlikComboBox->setCompleter(nullptr);    // nullptr yapmamın sebebi startCompleter'ın içinde delete edebilmek
    ui->adSoyadComboBox->setCompleter(nullptr);

    startCompleter(activityListFile, ui->etkinlikComboBox);
    startCompleter(participantListFile, ui->adSoyadComboBox);

    ui->etkinlikComboBox->setCurrentIndex(-1);
    ui->adSoyadComboBox->setCurrentIndex(-1);
}

Widget::~Widget()
{
    delete ui;
}
QAbstractItemModel *Widget::modelFromFile(QFile& file)
{
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;
    file.seek(0);
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << QString::fromUtf8(line.trimmed());
    }
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, completer);
}

void Widget::addToFile(QFile& file, QComboBox* comboBox)
{
    QTextStream stream(&file);
    if (file.fileName() == participantListFile.fileName())
    {
        auto categories = ui->yasKategoriGroupBox->findChildren<QRadioButton*>();
        auto checkedButton = std::find_if(std::cbegin(categories), std::cend(categories), [](const auto&button){return button->isChecked();});
        if(comboBox->currentText().isEmpty())
        {
            QMessageBox qmbox;
            qmbox.warning(nullptr, tr(appName), "Kişi ismi girilmemiş!");
            return;
        }
        else if (checkedButton == categories.cend())
        {
            QMessageBox qmbox;
            qmbox.warning(nullptr, tr(appName), QString("%1 için yaş kategorisi seçilmemiş!").arg(comboBox->currentText()));
            return;
        }
        else
        {
            statusBar->showMessage(QString("\"%1\" kategorisindeki \"%2\" kaydedildi").arg((*checkedButton)->text(), comboBox->currentText()));
            stream << comboBox->currentText() << " (" << (*checkedButton)->text() << ")\n";
        }
        (*checkedButton)->setAutoExclusive(false);
        (*checkedButton)->setChecked(false);
        (*checkedButton)->setAutoExclusive(true);
    }
    else
    {
        statusBar->showMessage(QString("\"%1\" kaydedildi").arg(comboBox->currentText()));
        stream << comboBox->currentText() << '\n';
    }
    stream.flush();
    comboBox->clearEditText();
    startCompleter(file, comboBox);
}
void Widget::startCompleter(QFile& file, QComboBox* comboBox)
{
    completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(10);
    completer->setWrapAround(true);     // bunun ne işe yaradığını anlamadım

    QAbstractItemModel* abstractItemModel = modelFromFile(file);
    completer->setModel(abstractItemModel);

//    QStringListModel list1 = static_cast<QStringListModel>(abstractItemModel);

    QStringList list = static_cast<QStringListModel*>(abstractItemModel)->stringList();
//    QStringListModel list1 = static_cast<QStringListModel>(abstractItemModel);
//    QStringList list = list1.stringList();
    comboBox->addItems(list);

    delete comboBox->completer();
    comboBox->setCompleter(completer);
}

void Widget::openFile(QFile& file, const char* fileName, QIODevice::OpenModeFlag flag)
{
    file.setFileName(fileName);

    if (!file.open(flag))
    {
        file.close();
        QMessageBox qmbox;
        qmbox.critical(nullptr, tr(appName), QString("%1 dosyası açılamadı!").arg(fileName));
    }
}

void Widget::highlightedIndex(int index)
{
    qDebug() << "index:" << index;
}

void Widget::highlightedString(const QString &text)
{
    qDebug() << "text:" << text;
}
