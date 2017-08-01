#ifndef SINGLEMUSICMIXER_H
#define SINGLEMUSICMIXER_H

#include <QFrame>
#include "audio/soundmanager.h"

namespace Ui {
class SingleMusicMixer;
}

class SingleMusicMixer : public QFrame
{
    Q_OBJECT
    
public:
    explicit SingleMusicMixer(QWidget *parent = 0);
    ~SingleMusicMixer();

    void linkToMusic(int id);
    void initialize(SoundManager* m);
    void update();
    static QString MimeType()  { return QStringLiteral("widget/SingleMusicMixer"); }
private slots:
    void on_verticalSlider_valueChanged(int value);
    
protected:
    void mousePressEvent( QMouseEvent *event ) override;
    void mouseReleaseEvent( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event ) override;
private:
    Ui::SingleMusicMixer *ui;
    SoundManager* m;
    int music_id;
    
    int last_progressBar_value=-1;
    
    bool mouse_pressed = false;
    QPoint mouse_pos;
};

#endif // SINGLEMUSICMIXER_H
