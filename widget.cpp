#include "ui_widget.h"
#include "widget.h"

#include <algorithm>

#include <QStringListModel>
#include <QCompleter>
#include <QStatusBar>
#include <QMenuBar>
#include <QDebug>
#include <QMenu>

QString appName = "Etkinlik Kayıt Programı";

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget), activity{std::make_unique<Activity>(this)}
{
    ui->setupUi(this);

    initializeMenusAndBars();

	getFromFile(*activity->participantListFile, participantList);
    participantsWidget = new ParticipantsWidget(&participantList);

    update.setParameters("https://api.github.com/repos/atakli/EtkinlikKayit/releases/latest", appName, "etkinlikKayit.zip");
    update.isNewVersionAvailable();

    connect(ui->etkinlikEklePushButton, &QPushButton::clicked, this, [this]{activity->addToActivityListFile(ui->etkinlikComboBox);});
    connect(ui->kisiEklePushButton, &QPushButton::clicked, this, [this]{activity->addToParticipantListFile(ui->adSoyadComboBox, ui->yasKategoriGroupBox->findChildren<QRadioButton*>());});
	connect(ui->katilimcilariGetirPushButton, &QPushButton::clicked, participantsWidget, &ParticipantsWidget::show);
	connect(ui->punishedPushButton, &QPushButton::clicked, activity.get(), &Activity::openPunishedList);

    connect(activity.get(), &Activity::statusBarMessage, this, [this](const QString& msg){statusBar->showMessage(msg);});
    connect(activity.get(), &Activity::addItemToParticipantsWidget, this, [this](const QString& msg){participantsWidget->addItem(msg);});
    connect(activity.get(), &Activity::startCompleter, this, &Widget::startCompleter);
	connect(ui->kaydetPushButton, &QPushButton::clicked, activity.get(), &Activity::addActivity);

//    connect(ui->adSoyadComboBox, &QComboBox::highlighted, this, &Widget::highlightedIndex);
//    connect(ui->adSoyadComboBox, &QComboBox::textHighlighted, this, &Widget::highlightedString);
//    connect(ui->adSoyadComboBox, &QComboBox::textActivated, this, &Widget::highlightedString);
//    connect(ui->adSoyadComboBox, &QComboBox::currentTextChanged, this, &Widget::highlightedString);

    ui->dateEdit->setDate(QDateTime::currentDateTime().date());

    ui->etkinlikComboBox->setCompleter(nullptr);    // nullptr yapmamın sebebi startCompleter'ın içinde delete edebilmek
    ui->adSoyadComboBox->setCompleter(nullptr);

	startCompleter(*activity->activityListFile, ui->etkinlikComboBox);
	startCompleter(*activity->participantListFile, ui->adSoyadComboBox);

    ui->etkinlikComboBox->setCurrentIndex(-1);
    ui->adSoyadComboBox->setCurrentIndex(-1);
}
Widget::~Widget()
{
	delete ui;
}

void Widget::startCompleter(QFile& file, QComboBox* comboBox)
{
    QCompleter* completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(10);
    completer->setWrapAround(true);     // bunun ne işe yaradığını anlamadım

    auto [stringList, stringListModel] = getFromFile(file, participantList);
    completer->setModel(stringListModel);

    comboBox->addItems(stringList);

    delete comboBox->completer();
    comboBox->setCompleter(completer);
}

void Widget::initializeMenusAndBars()
{
    QMenuBar* menuBar = new QMenuBar;

    QMenu* windowMenu = new QMenu("Window");
    menuBar->addMenu(windowMenu);
    QMenu* helpMenu = new QMenu("Yardım");
    menuBar->addMenu(helpMenu);
//    QMenu* aboutMenu = new QMenu("Hakkında");   // burda credits bolumu ekleyip soyle de: Icon by ToZIcon on freeicons.io
//    menuBar->addMenu(helpMenu);

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

void Widget::onTopAction()				// TODO: linux'ta bunda sıkıntı var. sadece biri oTop oldu
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
    if (participantsWidget->isActiveWindow())
        participantsWidget->show();
    show();
}

std::pair<QStringList, QStringListModel*> Widget::getFromFile(QFile& file, QStringList& participantList)
{
#ifndef QT_NO_CURSOR
	QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
	QStringList words;
	file.seek(0);
	while (!file.atEnd())
	{
		const QByteArray line = file.readLine();
		if (!line.isEmpty())
			words << QString::fromUtf8(line.trimmed());
	}
#ifndef QT_NO_CURSOR
	QGuiApplication::restoreOverrideCursor();
#endif

	QStringListModel* stringListModel = new QStringListModel(words, completer);
	QStringList stringList = stringListModel->stringList();
	if (file.fileName() == activity->participantListFile->fileName())
		participantList = stringList;
	participantList.sort(Qt::CaseInsensitive);
	return std::make_pair(stringList, stringListModel);
}
void Widget::highlightedIndex(int index)
{
    qDebug() << "index:" << index;
}

void Widget::highlightedString(const QString &text)
{
    qDebug() << "text:" << text;
}
