#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audio/soundmanager.h"
#include <QPushButton>
#include <QTimer>
#include <iostream>

SoundManager *s = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    s = new SoundManager(this);
    s->Initialize();
    s->OpenStream(nullptr);
    
    
    ui->music_controller->initialize(s);
    ui->music_controller->linkToMusic(0);
    
    s->Music_load(0, "PSY.ogg");
    s->Music_play(0);
    s->Music_setGain(0, 0);
    
    // End of Init
    QTimer *timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);
    
    
    SingleMusicMixer* music;
    music = new SingleMusicMixer(nullptr);
    music->linkToMusic(1);
    //music->setGeometry(0, 0, 100, 500);
    ui->reorder_view->addWidget(music);
    music = new SingleMusicMixer(nullptr);
    music->linkToMusic(2);
    ui->reorder_view->addWidget(music);
    music = new SingleMusicMixer(nullptr);
    music->linkToMusic(3);
    ui->reorder_view->addWidget(music);
    music = new SingleMusicMixer(nullptr);
    music->linkToMusic(4);
    ui->reorder_view->addWidget(music);
    music = new SingleMusicMixer(nullptr);
    music->linkToMusic(5);
    ui->reorder_view->addWidget(music);
    
    /*QPushButton *bouton = new QPushButton("Mon bouton entre en scène !");
    ui->reorder_view->addWidget(bouton);
    bouton->setMinimumHeight(222);
    bouton = new QPushButton("Mon bouton 2 !");
    ui->reorder_view->addWidget(bouton);
    bouton = new QPushButton("Mon bouton 3 !");
    ui->reorder_view->addWidget(bouton);
    bouton = new QPushButton("Mon bouton 4 !");
    ui->reorder_view->addWidget(bouton);*/
    
}

MainWindow::~MainWindow()
{
    delete ui;
    
    s->Terminate();
    delete s;
}

void MainWindow::update()
{
    ui->music_controller->update();
}

