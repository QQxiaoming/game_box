#include <QPixmap>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString VERSION = APP_VERSION;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    buff = new uint16_t[256*240];
    qImg = new QImage((const unsigned char*)(buff), 256, 240, QImage::Format_RGB555);

    memset(buff,0x0,256*240*2);
    this->setWindowTitle("NES Game");
    timer->start(33);

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timer_repaint()));
    QObject::connect(ui->action_open, SIGNAL(triggered()), this, SLOT(open_triggered()));
    QObject::connect(ui->action_close, SIGNAL(triggered()), this, SLOT(close_triggered()));
    QObject::connect(ui->action_about, SIGNAL(triggered()), this, SLOT(about_triggered()));
    QObject::connect(ui->action_sample_1, SIGNAL(triggered()), this, SLOT(sample_1_triggered()));
    QObject::connect(ui->action_sample_2, SIGNAL(triggered()), this, SLOT(sample_2_triggered()));
    QObject::connect(ui->action_sample_3, SIGNAL(triggered()), this, SLOT(sample_3_triggered()));
    QObject::connect(ui->action_sample_4, SIGNAL(triggered()), this, SLOT(sample_4_triggered()));
}

MainWindow::~MainWindow()
{
    delete qImg;
    delete[] buff;
    delete ui;
    if(nesThread != nullptr){
        delete nesThread;
        nesThread = nullptr;
    }
    delete timer;
}

void MainWindow::sample_1_triggered()
{
    QString file_name = ":/game/games/SuperMario.nes";
    if(nesThread != nullptr){
        delete nesThread;
        nesThread = nullptr;
    }
    this->setWindowTitle(file_name);
    nesThread = new NESThread(this,buff,file_name);
    nesThread->start();
}

void MainWindow::sample_2_triggered()
{
    QString file_name = ":/game/games/CatAndMouse.nes";
    if(nesThread != nullptr){
        delete nesThread;
        nesThread = nullptr;
    }
    this->setWindowTitle(file_name);
    nesThread = new NESThread(this,buff,file_name);
    nesThread->start();
}

void MainWindow::sample_3_triggered()
{
    QString file_name = ":/game/games/Tanks.nes";
    if(nesThread != nullptr){
        delete nesThread;
        nesThread = nullptr;
    }
    this->setWindowTitle(file_name);
    nesThread = new NESThread(this,buff,file_name);
    nesThread->start();
}

void MainWindow::sample_4_triggered()
{
    QString file_name = ":/game/games/MacrossSeries.nes";
    if(nesThread != nullptr){
        delete nesThread;
        nesThread = nullptr;
    }
    this->setWindowTitle(file_name);
    nesThread = new NESThread(this,buff,file_name);
    nesThread->start();
}


void MainWindow::open_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("选择文件"), "", "nes rom(*.nes)");
    if (file_name.isEmpty()) {
        return;
    }
    if(nesThread != nullptr){
        delete nesThread;
        nesThread = nullptr;
    }
    this->setWindowTitle(file_name);
    nesThread = new NESThread(this,buff,file_name);
    nesThread->start();
}

void MainWindow::close_triggered()
{
    if(nesThread != nullptr){
        delete nesThread;
        nesThread = nullptr;
        this->setWindowTitle("NES Game");
    }
}

void MainWindow::about_triggered()
{
    QMessageBox::about(this, "About", "版本 \n " + VERSION + "\n作者\n qiaoqm@aliyun.com");
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.drawPixmap(QPoint(0, 25), QPixmap::fromImage(qImg->scaled(this->size()-QSize(0,25))));
    painter.end();

    (void)event;
}

/**
 * FC手柄 bit 键位对应关系 真实手柄中有一个定时器，处理 连A  连B
 * 0  1   2       3       4    5      6     7
 * A  B   Select  Start  Up   Down   Left  Right
 */
void MainWindow::keyPressEvent(QKeyEvent * event) {
    if(nesThread == nullptr){
        return;
    }
    switch (event->key()) {
        case Qt::Key_Z: {
            nesThread->pdwPad1 |= (1UL<<0);
            break;
        }
        case Qt::Key_X: {
            nesThread->pdwPad1 |= (1UL<<1);
            break;
        }
        case Qt::Key_Control: {
            nesThread->pdwPad1 |= (1UL<<2);
            break;
        }
        case Qt::Key_Return: {
            nesThread->pdwPad1 |= (1UL<<3);
            break;
        }
        case Qt::Key_Up: {
            nesThread->pdwPad1 |= (1UL<<4);
            break;
        }
        case Qt::Key_Down: {
            nesThread->pdwPad1 |= (1UL<<5);
            break;
        }
        case Qt::Key_Left: {
            nesThread->pdwPad1 |= (1UL<<6);
            break;
        }
        case Qt::Key_Right: {
            nesThread->pdwPad1 |= (1UL<<7);
            break;
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent * event) {
    if(nesThread == nullptr){
        return;
    }
    switch (event->key()) {
        case Qt::Key_Z: {
            nesThread->pdwPad1 &= ~(1UL<<0);
            break;
        }
        case Qt::Key_X: {
            nesThread->pdwPad1 &= ~(1UL<<1);
            break;
        }
        case Qt::Key_Control: {
            nesThread->pdwPad1 &= ~(1UL<<2);
            break;
        }
        case Qt::Key_Return: {
            nesThread->pdwPad1 &= ~(1UL<<3);
            break;
        }
        case Qt::Key_Up: {
            nesThread->pdwPad1 &= ~(1UL<<4);
            break;
        }
        case Qt::Key_Down: {
            nesThread->pdwPad1 &= ~(1UL<<5);
            break;
        }
        case Qt::Key_Left: {
            nesThread->pdwPad1 &= ~(1UL<<6);
            break;
        }
        case Qt::Key_Right: {
            nesThread->pdwPad1 &= ~(1UL<<7);
            break;
        }
    }
}

void MainWindow::timer_repaint()
{
    this->repaint();
}

NESThread::NESThread(QObject *parent,uint16_t* buff,QString pszFileName) :
    QThread(parent)
{
    workFrame = buff;
    fileName = new QByteArray(pszFileName.toUtf8().data(),pszFileName.toUtf8().size());
}

NESThread::~NESThread() {
    this->pdwSystem = 1;
    requestInterruption();
    quit();
    wait();
    delete fileName;
    memset(workFrame,0x0,256*240*2);
}

void NESThread::run()
{
    extern void InfoNES_start(NESThread *nesThread,const char *pszFileName);
    InfoNES_start(this,fileName->data());
}

int NESThread::InfoNES_OpenRom(const char *pszFileName)
{
    if(file != nullptr) {
        if (file->isOpen()) {
            return (int)file->size();
        }
    }
    file = new QFile(pszFileName);
    if(file->open(QFile::ReadOnly)) {
        return (int)file->size();
    } else {
        return -1;
    }
}

int NESThread::InfoNES_ReadRom(void *buf, unsigned int len)
{
    return (int)file->read((char *)buf,len);
}

void NESThread::InfoNES_CloseRom(void)
{
    if(file->isOpen()) {
        file->close();
        delete file;
        file = nullptr;
    }
}

void NESThread::InfoNES_Wait(void)
{
#if 1
    this->usleep(1);
#else
    for (volatile int i = 0; i < 20; i++)
        for (volatile int j = 0; j < 1000; j++)
            ;
#endif
}

void NESThread::InfoNES_LoadFrame(void)
{
    //TODO:
}

void NESThread::InfoNES_PadState(uint32_t *pdwPad1, uint32_t *pdwPad2, uint32_t *pdwSystem)
{
    *pdwPad1 = this->pdwPad1;
    *pdwPad2 = this->pdwPad2;
    *pdwSystem = this->pdwSystem;
}

void NESThread::InfoNES_SoundInit(void)
{
    //TODO:
}

int NESThread::InfoNES_SoundOpen(int samples_per_sync, int sample_rate)
{
    //TODO:
    (void)samples_per_sync;
    (void)sample_rate;
    return -1;
}

void NESThread::InfoNES_SoundClose(void)
{
    //TODO:
}

void NESThread::InfoNES_SoundOutput(int samples, uint8_t *wave1, uint8_t *wave2, uint8_t *wave3,
                         uint8_t *wave4, uint8_t *wave5)
{
    //TODO:
    (void)samples;
    (void)wave1;
    (void)wave2;
    (void)wave3;
    (void)wave4;
    (void)wave5;
}

void NESThread::InfoNES_MessageBox(char *buf)
{
    qDebug() << buf;
}


