#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audio/soundmanager.h"
#include <QPushButton>
#include <QTimer>
#include <iostream>
#include "_libs/miniz/miniz.h"

SoundManager *s = nullptr;

void test_miniz(){
    typedef unsigned char uint8;
    typedef unsigned short uint16;
    typedef unsigned int uint;
    
    static const char *s_pStr = "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
      "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
      "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
      "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
      "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
      "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson." \
      "Good morning Dr. Chandra. This is Hal. I am ready for my first lesson.";
    
    uint step = 0;
    int cmp_status;
    uLong src_len = (uLong)strlen(s_pStr);
    uLong cmp_len = compressBound(src_len);
    uLong uncomp_len = src_len;
    uint8 *pCmp, *pUncomp;
    uint total_succeeded = 0;
        
    printf("miniz.c version: %s\n", MZ_VERSION);
    
    do
    {
        // Allocate buffers to hold compressed and uncompressed data.
        pCmp = (mz_uint8 *)malloc((size_t)cmp_len);
        pUncomp = (mz_uint8 *)malloc((size_t)src_len);
        if ((!pCmp) || (!pUncomp))
        {
            printf("Out of memory!\n");
        }
        
        // Compress the string.
        cmp_status = compress(pCmp, &cmp_len, (const unsigned char *)s_pStr, src_len);
        if (cmp_status != Z_OK)
        {
            printf("compress() failed!\n");
            free(pCmp);
            free(pUncomp);
        }
        
        printf("Compressed from %u to %u bytes\n", (mz_uint32)src_len, (mz_uint32)cmp_len);
        
        if (step)
        {
            // Purposely corrupt the compressed data if fuzzy testing (this is a very crude fuzzy test).
            uint n = 1 + (rand() % 3);
            while (n--)
            {
                uint i = rand() % cmp_len;
                pCmp[i] ^= (rand() & 0xFF);
            }
        }
        
        // Decompress.
        cmp_status = uncompress(pUncomp, &uncomp_len, pCmp, cmp_len);
        total_succeeded += (cmp_status == Z_OK);
        
        if (step)
        {
            printf("Simple fuzzy test: step %u total_succeeded: %u\n", step, total_succeeded);
        }
        else
        {
            if (cmp_status != Z_OK)
            {
                printf("uncompress failed!\n");
                free(pCmp);
                free(pUncomp);
            }
            
            printf("Decompressed from %u to %u bytes\n", (mz_uint32)cmp_len, (mz_uint32)uncomp_len);
            
            // Ensure uncompress() returned the expected data.
            if ((uncomp_len != src_len) || (memcmp(pUncomp, s_pStr, (size_t)src_len)))
            {
                printf("Decompression failed!\n");
                free(pCmp);
                free(pUncomp);
            }
        }
        
        free(pCmp);
        free(pUncomp);
        
        step++;
        
        // Keep on fuzzy testing if there's a non-empty command line.
    } while (false);
    
    printf("Success.\n");
    std::cout << std::endl;
}


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
    
    test_miniz();
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

