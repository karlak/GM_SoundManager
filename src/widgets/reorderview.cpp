#include "reorderview.h"

#include <QGraphicsProxyWidget>
#include <QPushButton>
#include <QLayout>
#include <iostream>
#include <algorithm>
#include <QMimeData>
#include <QScrollBar>

#include "singlemusicmixer.h"

#define ECART_WIDGET_MOVING 30
#define ECART_BETWEEN_WIDGETS 2

ReorderView::ReorderView(QWidget *parent) :
    QGraphicsView(parent)
{
    setScene(&scene);
    resizeEvent(nullptr);
    setAlignment(Qt::AlignLeft|Qt::AlignTop);
    connect(&scene, &QGraphicsScene::sceneRectChanged, this, &ReorderView::sceneRectChanged);

    QColor c(240, 240, 240, 255);
    QBrush b(c);
    scene.setBackgroundBrush(b);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

ReorderView::~ReorderView()
{
}

void ReorderView::addWidget(QWidget* w)
{
    w;
    
    //static float curr_x = 0;
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
    proxy->setWidget(w);
    list.append(proxy);
    
    //proxy->setPos(curr_x, 0);
    //w->setGeometry(curr_x, 0, w->sizeHint().width(), w->sizeHint().height());
    //curr_x += w->width();
    scene.addItem(proxy);
    
}


void ReorderView::updateSceneRect(const QRectF& rect)
{
    rect;
    auto sceneRect = scene.itemsBoundingRect();//scene.sceneRect();
    sceneRect.setX(0);
    sceneRect.setY(0);

    setSceneRect(sceneRect);
    updateGeometry();
}



void ReorderView::dragMoveEvent(QDragMoveEvent* event)
{
    event;
    if (event->mimeData()->hasFormat(SingleMusicMixer::MimeType())) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
        
        QByteArray pieceData = event->mimeData()->data(SingleMusicMixer::MimeType());
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPoint mouse_pos_old;
        SingleMusicMixer* widgetMoved;
        quint64 tmp_widget;
        dataStream >> tmp_widget >> mouse_pos_old;
        widgetMoved = (SingleMusicMixer*)tmp_widget;
        
        QPoint mouse_pos_current = event->pos();
        
        float curr_x = ECART_WIDGET_MOVING;
        foreach (auto proxy, list) {
            auto w = proxy->widget();
            if(w == widgetMoved){
                proxy->setPos(mouse_pos_current.x()-w->width()/2, 0);
                proxy->setOpacity(0.5);
                proxy->setZValue(1);
                curr_x += w->width()-ECART_WIDGET_MOVING*2 + ECART_BETWEEN_WIDGETS;
                continue;
            }
            
            if(curr_x+w->width()/2 < mouse_pos_current.x()){
                proxy->setPos(curr_x-ECART_WIDGET_MOVING, 0);
            }
            else{
                proxy->setPos(curr_x+ECART_WIDGET_MOVING, 0);
            }
            
            curr_x += w->width() + ECART_BETWEEN_WIDGETS;
        }
        
    } else {
        event->ignore();
    }
}

void ReorderView::dragLeaveEvent(QDragLeaveEvent*)
{
    updateLayout();
}

void ReorderView::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(SingleMusicMixer::MimeType())) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
        
        QByteArray pieceData = event->mimeData()->data(SingleMusicMixer::MimeType());
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPoint mouse_pos_old;
        SingleMusicMixer* widgetMoved;
        quint64 tmp_widget;
        dataStream >> tmp_widget >> mouse_pos_old;
        widgetMoved = (SingleMusicMixer*)tmp_widget;
        QPoint mouse_pos_current = event->pos();
        
        int idFrom = -1;
        
        for(int i=0;i<list.size();i++){
            if(list.at(i)->widget() == widgetMoved)
                idFrom = i;
        }
        if(idFrom>=0){
            int idTo = 0;
            float curr_x = ECART_WIDGET_MOVING;
            foreach (auto proxy, list) {
                auto w = proxy->widget();
                if(w == widgetMoved){
                    curr_x += w->width()-ECART_WIDGET_MOVING*2 + ECART_BETWEEN_WIDGETS;
                    continue;
                }
                idTo++;
                
                if(curr_x+w->width()/2 > mouse_pos_current.x()){
                    idTo -=1;
                    break;
                }
                curr_x += w->width() + ECART_BETWEEN_WIDGETS;
            }
            
            if(idTo>=0){
                auto item = list.takeAt(idFrom);
                list.insert(idTo, item);
            }
        }
        
    } else {
        event->ignore();
    }
    updateLayout();
}

void ReorderView::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
}

void ReorderView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
}

QSize ReorderView::sizeHint() const
{
    return QSize(sceneRect().width(), sceneRect().height());
}

void ReorderView::sceneRectChanged(const QRectF& rect)
{
    updateSceneRect(rect);
}



void ReorderView::updateLayout()
{
    updateSceneRect(scene.sceneRect());
    float curr_x = 0;
    foreach (auto proxy, list) {
        auto w = proxy->widget();
        proxy->setOpacity(1);
        proxy->setZValue(0);
        //w->setGeometry(curr_x, 0, w->sizeHint().width(), w->sizeHint().height());
        proxy->setPos(curr_x, 0);
        curr_x += w->width() + ECART_BETWEEN_WIDGETS;
    }
}
void ReorderView::resizeEvent(QResizeEvent*)
{
    updateLayout();
}












