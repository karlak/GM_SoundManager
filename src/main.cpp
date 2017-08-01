#include "mainwindow.h"
#include <QApplication>

#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>
#define GetCurrentDir _getcwd
#include <iostream>

char cCurrentPath[FILENAME_MAX];



int main(int argc, char *argv[])
{
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
    {
        return errno;
    }
    
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
    
    std::cout << "The current working directory is " << cCurrentPath << std::endl;


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
