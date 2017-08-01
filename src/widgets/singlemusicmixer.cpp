#include "singlemusicmixer.h"
#include "ui_singlemusicmixer.h"
#include <QMouseEvent>
#include <iostream>
#include <QtGui>

SingleMusicMixer::SingleMusicMixer(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::SingleMusicMixer)
{
    this->m = nullptr;
    ui->setupUi(this);
    
}

SingleMusicMixer::~SingleMusicMixer()
{
    delete ui;
}

void SingleMusicMixer::linkToMusic(int id)
{
    music_id = id;
    ui->dockWidget->setWindowTitle(
                QString("Music ").append(std::to_string(id).c_str()));
}

void SingleMusicMixer::initialize(SoundManager* m)
{
    this->m = m;
}

void SingleMusicMixer::update()
{
    if(this->m){
        int curr = (int)(m->Music_getGain(music_id)*100);
        if(last_progressBar_value != curr){
            ui->progressBar->setValue(curr);
            ui->progressBar->repaint();
            last_progressBar_value = curr;
        }
    }
}

void SingleMusicMixer::on_verticalSlider_valueChanged(int value)
{
    if(this->m){
        m->Music_setGain(music_id, value/100.f);
    }
}

void SingleMusicMixer::mousePressEvent(QMouseEvent* event)
{
    QWidget * w = childAt(event->x(), event->y());
    if(w == ui->dockWidget){
        mouse_pressed = true;
        mouse_pos = event->globalPos();
    }
}

void SingleMusicMixer::mouseReleaseEvent(QMouseEvent*)
{
    mouse_pressed = false;    
}

void SingleMusicMixer::mouseMoveEvent(QMouseEvent* event)
{
    if(mouse_pressed){
        auto diff = event->globalPos() - mouse_pos;
        if(diff.manhattanLength()>10){
            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);  
            quint64 tmp_this = (quint64) this;
            dataStream << tmp_this << mouse_pos;
            
            QMimeData *mimeData = new QMimeData;
            mimeData->setData(SingleMusicMixer::MimeType(), itemData);
        
            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->setHotSpot(QPoint(0, 0));
        
            if (drag->exec(Qt::MoveAction) == Qt::MoveAction){
            }
            mouse_pressed = false;
                
        }
    }
    else{
        event->ignore();
    }
    
}
