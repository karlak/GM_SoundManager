#ifndef REORDERVIEW_H
#define REORDERVIEW_H

#include <QFrame>
#include <QGraphicsView>
#include <QGridLayout>


class ReorderView : public QGraphicsView
{
    Q_OBJECT
    
public:
    explicit ReorderView(QWidget *parent = 0);
    ~ReorderView();
    void addWidget(QWidget* w);
private:
    QGraphicsScene scene;
    QGridLayout m_layout;
    QList<QGraphicsProxyWidget*> list;
    void resizeEvent(QResizeEvent*) override;    
    void updateSceneRect(const QRectF& rect);
    void updateLayout();
protected:
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event ) override;
    virtual QSize sizeHint() const override;

public slots:
    void sceneRectChanged(const QRectF &rect);
};

#endif // REORDERVIEW_H
