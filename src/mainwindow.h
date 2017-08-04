#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "audio/soundmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void update();
    void clickMenuAudioOut();
    
private:
    Ui::MainWindow *ui;
    std::vector<DeviceInfo> devices;
};

#endif // MAINWINDOW_H
