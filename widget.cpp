#include "ui_widget.h"
#include "widget.h"

#include <algorithm>

#include <QStringListModel>
#include <QMessageBox>
#include <QCompleter>
#include <QStatusBar>
#include <QMenuBar>
#include <QDebug>
#include <QMenu>

extern QString applicationDirPath;

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    initializeMenusAndBars();

    openFile(activityListFile, "etkinlikler.txt", QIODevice::ReadWrite);
    openFile(logFile, "logs.txt", QIODevice::ReadWrite);
    openFile(participantListFile, "katilimcilar.txt", QIODevice::ReadWrite);

    getFromFile(participantListFile);

    participantsWidget = new ParticipantsWidget(&participantList);

    update.setParameters("https://api.github.com/repos/atakli/EtkinlikKayit/releases/latest", appName, "etkinlikKayit.zip");
    update.isNewVersionAvailable();

    connect(ui->etkinlikEklePushButton, &QPushButton::clicked, this, [this](){this->addToFile(activityListFile, ui->etkinlikComboBox);});
    connect(ui->kisiEklePushButton, &QPushButton::clicked, this, [this](){this->addToFile(participantListFile, ui->adSoyadComboBox);});
    connect(ui->katilimcilariGetirPushButton, &QPushButton::clicked, participantsWidget, &ParticipantsWidget::show);
    connect(ui->kaydetPushButton, &QPushButton::clicked, this, &Widget::addActivity);

//    connect(ui->adSoyadComboBox, &QComboBox::highlighted, this, &Widget::highlightedIndex);
//    connect(ui->adSoyadComboBox, &QComboBox::textHighlighted, this, &Widget::highlightedString);
//    connect(ui->adSoyadComboBox, &QComboBox::textActivated, this, &Widget::highlightedString);
//    connect(ui->adSoyadComboBox, &QComboBox::currentTextChanged, this, &Widget::highlightedString);

    ui->dateEdit->setDate(QDateTime::currentDateTime().date());

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

QStringListModel *Widget::modelFromFile(QFile& file)
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

void Widget::getFromFile(QFile& file)
{
	stringListModel = modelFromFile(file);
    stringList = stringListModel->stringList();
    if (file.fileName() == participantListFile.fileName())
        participantList = stringList;
    participantList.sort(Qt::CaseInsensitive);
}

QStringList Widget::getLastThreeActivityDates(const QString& etkinlikFileName)
{
    QFile file(etkinlikFileName);
    openFile(file, etkinlikFileName + ".txt", QIODevice::ReadWrite);
    // vektöre veya set'e atıp sıralayıp sonra son üçünü alabilirim. hatta son üçünü almak için tamamını sıralamama gerek olmayabilri. bunun bahsi geçmişti sanki
    return {};
}

void Widget::addToFile(QFile& file, QComboBox* comboBox)
{
    QTextStream stream(&file);
	QMessageBox qmbox;
    if (file.fileName() == participantListFile.fileName())
    {
        auto categories = ui->yasKategoriGroupBox->findChildren<QRadioButton*>();
        auto checkedButtonIter = std::find_if(std::cbegin(categories), std::cend(categories), [](const auto&button){return button->isChecked();});
        /*Warns when a lambda inside a connect() captures local variables by reference.
        Example:
        int a;
        connect(obj, &MyObj::mySignal, [&a] { ... });
        This usually results in a crash since the lambda might get called after the captured variable went out of scope.*/
        checkedButton = *checkedButtonIter;
        if(comboBox->currentText().isEmpty())
        {
            qmbox.warning(nullptr, tr(appName), "Kişi ismi girilmemiş!");
            return;
        }
        else if (checkedButtonIter == categories.cend())
        {
            qmbox.warning(nullptr, tr(appName), QString("%1 için yaş kategorisi seçilmemiş!").arg(comboBox->currentText()));
            return;
        }

		statusBar->showMessage(QString("\"%1\" kategorisindeki \"%2\" kaydedildi").arg(checkedButton->text(), comboBox->currentText()));
		stream << comboBox->currentText() << " (" << checkedButton->text() << ")\n";

		checkedButton->setAutoExclusive(false);
		checkedButton->setChecked(false);
		checkedButton->setAutoExclusive(true);

        participantsWidget->addItem(comboBox->currentText() + " (" + checkedButton->text() + ")");
    }
    else
    {
		if(comboBox->currentText().isEmpty())
		{
			qmbox.warning(nullptr, tr(appName), "Etkinlik ismi girilmemiş!");
			return;
		}
        statusBar->showMessage(QString("\"%1\" kaydedildi").arg(comboBox->currentText()));
        stream << comboBox->currentText() << '\n';
    }
    stream.flush();
    startCompleter(file, comboBox);

	comboBox->clearEditText();
}
void Widget::startCompleter(QFile& file, QComboBox* comboBox)
{
    completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(10);
    completer->setWrapAround(true);     // bunun ne işe yaradığını anlamadım

    getFromFile(file);
    completer->setModel(stringListModel);

    comboBox->addItems(stringList);

    delete comboBox->completer();
    comboBox->setCompleter(completer);
}

void Widget::openFile(QFile& file, const QString& fileName, QIODevice::OpenModeFlag flag)       // error return ekle veya exception
{
    file.setFileName(fileName);

    if (!file.open(flag))
    {
        file.close();
        QMessageBox qmbox;
        qmbox.critical(nullptr, tr(appName), QString("%1 dosyası açılamadı!").arg(fileName));
    }
}

void Widget::addActivity()
{
    QString etkinlikFileName = ui->etkinlikComboBox->currentText();

    if (etkinlikFileName.isEmpty())
    {
        QMessageBox qmbox;
        qmbox.warning(nullptr, tr(appName), QString("Etkinlik ismi girmediniz!"));
        return;
    }
    QString date = ui->dateEdit->text();                            // TODO: bunun da düzgün bi tarih olup olmadığını kontrol edeyim
    QFile etkinlikFile(etkinlikFileName);

    openFile(etkinlikFile, etkinlikFileName + ".txt", QIODevice::ReadWrite);

    etkinlikFile.seek(etkinlikFile.size());
    QTextStream stream(&etkinlikFile);
    auto selectedParticipants = participantsWidget->getSelectedParticipants();
    QStringList participants;
    if (selectedParticipants.empty())
    {
        QMessageBox qmbox;
        qmbox.warning(nullptr, tr(appName), QString("Katılımcı seçmediniz!"));
        return;
    }
    else
        for (auto index : selectedParticipants)
        {
            stream << date << ", " << participantList.at(index) << "\n";
            participants << participantList.at(index);
        }

    addActivityParticipant(etkinlikFileName, participants);
    stream.flush();
    statusBar->showMessage(QString("Seçili kişi(ler) \"%1\" etkinliğine kaydedildi").arg(etkinlikFileName));
}

void Widget::addActivityParticipant(const QString& fileName, const QStringList& selectedParticipants)
{
    QString etkinlikKatilimcilarilariFileName = fileName + "_Participants";
    QFile etkinlikKatilimcilariFile(etkinlikKatilimcilarilariFileName);
    openFile(etkinlikKatilimcilariFile, etkinlikKatilimcilarilariFileName + ".txt", QIODevice::ReadWrite);

    QStringList participants;
    etkinlikKatilimcilariFile.seek(0);
    while (!etkinlikKatilimcilariFile.atEnd())
    {
        QByteArray line = etkinlikKatilimcilariFile.readLine();
        if (!line.isEmpty())
            participants << QString::fromUtf8(line.trimmed());
    }
    for (const auto& participant : selectedParticipants)
    {
        auto iter = std::find(participants.begin(), participants.end(), participant);
        if (iter == participants.end())
        {
            participants << participant + ",3\n";
        }
        else
        {
            int number = (*iter).split(",").last().toInt();
            *iter = participant + "," + QString::number(++number) + "\n";
        }
    }
    etkinlikKatilimcilariFile.seek(0);                                  // TODO: sadece orayı değiştirmek diye bişey var mı? çünkü şuan baştan yazıyorum sanırım
    QTextStream stream(&etkinlikKatilimcilariFile);
    for (const auto& participant : participants)
        stream << participant;
    stream.flush();
}

void Widget::initializeMenusAndBars()
{
    QMenuBar* menuBar = new QMenuBar;

    QMenu* windowMenu = new QMenu("Window");
    menuBar->addMenu(windowMenu);
    QMenu* helpMenu = new QMenu("Yardım");
    menuBar->addMenu(helpMenu);

    QAction* onTopAct = new QAction("Her zaman üstte", this);
    onTopAct->setCheckable(true);
    windowMenu->addAction(onTopAct);
    connect(onTopAct, &QAction::triggered, this, &Widget::onTopAction);

    QAction* updateAct = new QAction("Güncelleme kontrolü", this);
    helpMenu->addAction(updateAct);
    connect(updateAct, &QAction::triggered, this, [this](){update.isNewVersionAvailable();});

    ui->verticalLayout_2->setMenuBar(menuBar);

    statusBar = new QStatusBar(this);
    ui->horizontalLayoutBottom->addWidget(statusBar);
}

void Widget::onTopAction()
{
    static bool alwaysOnTop = false;
    if (!alwaysOnTop)
    {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        participantsWidget->setWindowFlags(participantsWidget->windowFlags() | Qt::WindowStaysOnTopHint);
    }
    else
    {
        setWindowFlags(windowFlags() & (~Qt::WindowStaysOnTopHint));
        participantsWidget->setWindowFlags(participantsWidget->windowFlags() & (~Qt::WindowStaysOnTopHint));
    }
    alwaysOnTop = !alwaysOnTop;
    participantsWidget->show();
    show();
}

void Widget::highlightedIndex(int index)
{
    qDebug() << "index:" << index;
}

void Widget::highlightedString(const QString &text)
{
    qDebug() << "text:" << text;
}
