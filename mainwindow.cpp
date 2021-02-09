#include <QPixmap>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#define SOUND_NUM_FARME 2

const QString VERSION = APP_VERSION;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    buff = new uchar[256 * 240 * 2];
    qImg = new QImage(buff, 256, 240, QImage::Format_RGB555);

    memset(buff, 0x0, 256 * 240 * 2);
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
    QObject::connect(ui->action_mute, SIGNAL(triggered()), this, SLOT(mute_triggered()));
}

MainWindow::~MainWindow()
{
    if (nesThread != nullptr)
    {
        delete nesThread;
        nesThread = nullptr;
    }
    delete qImg;
    delete[] buff;
    delete timer;
    delete ui;
}

void MainWindow::start_nesThread(QString file_name)
{
    if (nesThread != nullptr)
    {
        delete nesThread;
        nesThread = nullptr;
    }
    this->setWindowTitle(file_name);
    nesThread = new NESThread(this, buff, file_name);
    nesThread->start();
}

void MainWindow::sample_1_triggered()
{
    QString file_name = ":/game/games/SuperMario.nes";
    start_nesThread(file_name);
}

void MainWindow::sample_2_triggered()
{
    QString file_name = ":/game/games/CatAndMouse.nes";
    start_nesThread(file_name);
}

void MainWindow::sample_3_triggered()
{
    QString file_name = ":/game/games/Tanks.nes";
    start_nesThread(file_name);
}

void MainWindow::sample_4_triggered()
{
    QString file_name = ":/game/games/MacrossSeries.nes";
    start_nesThread(file_name);
}

void MainWindow::open_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("选择文件"), "", "nes rom(*.nes)");
    if (file_name.isEmpty())
    {
        return;
    }
    start_nesThread(file_name);
}

void MainWindow::close_triggered()
{
    if (nesThread != nullptr)
    {
        delete nesThread;
        nesThread = nullptr;
        this->setWindowTitle("NES Game");
    }
}

void MainWindow::mute_triggered()
{
    static bool mute = true;
    ui->action_mute->setChecked(mute);
    nesThread->setMute(mute);
    mute = !mute;
}

void MainWindow::about_triggered()
{
    QMessageBox::about(this, "About", "版本 \n " + VERSION + "\n作者\n qiaoqm@aliyun.com");
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.drawPixmap(QPoint(0, 25), QPixmap::fromImage(qImg->scaled(this->size() - QSize(0, 25))));
    painter.end();

    Q_UNUSED(event);
}

/**
 * FC手柄 bit 键位对应关系 真实手柄中有一个定时器，处理 连A  连B
 * 0  1   2       3       4    5      6     7
 * A  B   Select  Start  Up   Down   Left  Right
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (nesThread == nullptr)
    {
        return;
    }
    switch (event->key())
    {
        case Qt::Key_Period:
        {
            nesThread->pdwPad1 |= (1UL << 0);
            break;
        }
        case Qt::Key_Slash:
        {
            nesThread->pdwPad1 |= (1UL << 1);
            break;
        }
        case Qt::Key_Control:
        {
            nesThread->pdwPad1 |= (1UL << 2);
            break;
        }
        case Qt::Key_Return:
        {
            nesThread->pdwPad1 |= (1UL << 3);
            break;
        }
        case Qt::Key_W:
        {
            nesThread->pdwPad1 |= (1UL << 4);
            break;
        }
        case Qt::Key_S:
        {
            nesThread->pdwPad1 |= (1UL << 5);
            break;
        }
        case Qt::Key_A:
        {
            nesThread->pdwPad1 |= (1UL << 6);
            break;
        }
        case Qt::Key_D:
        {
            nesThread->pdwPad1 |= (1UL << 7);
            break;
        }
        case Qt::Key_1:
        {
            nesThread->pdwPad2 |= (1UL << 0);
            break;
        }
        case Qt::Key_2:
        {
            nesThread->pdwPad2 |= (1UL << 1);
            break;
        }
        case Qt::Key_Plus:
        {
            nesThread->pdwPad2 |= (1UL << 2);
            break;
        }
        case Qt::Key_Enter:
        {
            nesThread->pdwPad2 |= (1UL << 3);
            break;
        }
        case Qt::Key_Up:
        {
            nesThread->pdwPad2 |= (1UL << 4);
            break;
        }
        case Qt::Key_Down:
        {
            nesThread->pdwPad2 |= (1UL << 5);
            break;
        }
        case Qt::Key_Left:
        {
            nesThread->pdwPad2 |= (1UL << 6);
            break;
        }
        case Qt::Key_Right:
        {
            nesThread->pdwPad2 |= (1UL << 7);
            break;
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (nesThread == nullptr)
    {
        return;
    }
    switch (event->key())
    {
        case Qt::Key_Period:
        {
            nesThread->pdwPad1 &= ~(1UL << 0);
            break;
        }
        case Qt::Key_Slash:
        {
            nesThread->pdwPad1 &= ~(1UL << 1);
            break;
        }
        case Qt::Key_Control:
        {
            nesThread->pdwPad1 &= ~(1UL << 2);
            break;
        }
        case Qt::Key_Return:
        {
            nesThread->pdwPad1 &= ~(1UL << 3);
            break;
        }
        case Qt::Key_W:
        {
            nesThread->pdwPad1 &= ~(1UL << 4);
            break;
        }
        case Qt::Key_S:
        {
            nesThread->pdwPad1 &= ~(1UL << 5);
            break;
        }
        case Qt::Key_A:
        {
            nesThread->pdwPad1 &= ~(1UL << 6);
            break;
        }
        case Qt::Key_D:
        {
            nesThread->pdwPad1 &= ~(1UL << 7);
            break;
        }
        case Qt::Key_1:
        {
            nesThread->pdwPad2 &= ~(1UL << 0);
            break;
        }
        case Qt::Key_2:
        {
            nesThread->pdwPad2 &= ~(1UL << 1);
            break;
        }
        case Qt::Key_Plus:
        {
            nesThread->pdwPad2 &= ~(1UL << 2);
            break;
        }
        case Qt::Key_Enter:
        {
            nesThread->pdwPad2 &= ~(1UL << 3);
            break;
        }
        case Qt::Key_Up:
        {
            nesThread->pdwPad2 &= ~(1UL << 4);
            break;
        }
        case Qt::Key_Down:
        {
            nesThread->pdwPad2 &= ~(1UL << 5);
            break;
        }
        case Qt::Key_Left:
        {
            nesThread->pdwPad2 &= ~(1UL << 6);
            break;
        }
        case Qt::Key_Right:
        {
            nesThread->pdwPad2 &= ~(1UL << 7);
            break;
        }
    }
}

void MainWindow::timer_repaint()
{
    this->repaint();
}

NESThread::NESThread(QObject *parent, void *buff, QString pszFileName) : QThread(parent)
{
    workFrame = static_cast<uint16_t *>(buff);
    fileName = new QByteArray(pszFileName.toUtf8().data(), pszFileName.toUtf8().size());
}

NESThread::~NESThread()
{
    this->pdwSystem = 1;
    requestInterruption();
    quit();
    wait();
    memset(workFrame, 0x0, 256 * 240 * 2);
    delete fileName;
}

void NESThread::run()
{
    extern void InfoNES_start(NESThread * nesThread, const char *pszFileName);
    InfoNES_start(this, fileName->data());
}

void NESThread::setMute(bool mute)
{
    m_mute = mute;
}

int NESThread::InfoNES_OpenRom(const char *pszFileName)
{
    if (file != nullptr)
    {
        if (file->isOpen())
        {
            return static_cast<int>(file->size());
        }
    }
    file = new QFile(pszFileName);
    if (file->open(QFile::ReadOnly))
    {
        return static_cast<int>(file->size());
    }
    else
    {
        return -1;
    }
}

int NESThread::InfoNES_ReadRom(void *buf, unsigned int len)
{
    return static_cast<int>(file->read(static_cast<char *>(buf), len));
}

void NESThread::InfoNES_CloseRom(void)
{
    if (file->isOpen())
    {
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
}

void NESThread::InfoNES_PadState(uint32_t *pdwPad1, uint32_t *pdwPad2, uint32_t *pdwSystem)
{
    *pdwPad1 = this->pdwPad1;
    *pdwPad2 = this->pdwPad2;
    *pdwSystem = this->pdwSystem;
}

void NESThread::InfoNES_SoundInit(void)
{
    audioFormat = new QAudioFormat();
    audioFormat->setChannelCount(1);
    audioFormat->setSampleSize(8);
    audioFormat->setCodec("audio/pcm");
    audioFormat->setByteOrder(QAudioFormat::LittleEndian);
    audioFormat->setSampleType(QAudioFormat::UnSignedInt);
}

int NESThread::InfoNES_SoundOpen(int samples_per_sync, int sample_rate)
{
    audioFormat->setSampleRate(sample_rate);
    audio = new QAudioOutput(*audioFormat, nullptr);
    audio_buff = new uchar[samples_per_sync * SOUND_NUM_FARME];
    memset(audio_buff, 0x0, (size_t)samples_per_sync * SOUND_NUM_FARME);
    audio->setBufferSize(samples_per_sync * 10);
    audio_dev = audio->start();
    return 0;
}

void NESThread::InfoNES_SoundClose(void)
{
    delete audio;
    delete audioFormat;
    delete[] audio_buff;
}

void NESThread::InfoNES_SoundOutput(int samples, uint8_t *wave1, uint8_t *wave2, uint8_t *wave3,
                                    uint8_t *wave4, uint8_t *wave5)
{
    static int index = 0;
    for (int i = 0; i < samples; i++)
    {
        uint32_t wav = ((uint32_t)wave1[i] + (uint32_t)wave2[i] + (uint32_t)wave3[i] + (uint32_t)wave4[i] + (uint32_t)wave5[i]) / 5UL;
        if(m_mute)
        {
            audio_buff[i + index * samples] = 0;
        }
        else
        {
            audio_buff[i + index * samples] = (uchar)wav;
        }
    }
    if (index < SOUND_NUM_FARME - 1)
    {
        index++;
    }
    else
    {
        int len = 0;
        forever
        {
            int n = audio_dev->write((char *)&audio_buff[len], samples * SOUND_NUM_FARME - len);
            if (n == -1)
            {
                break;
            }
            else
            {
                len += n;
                if (samples * SOUND_NUM_FARME == len)
                    break;
            }
        }
        index = 0;
    }
}

void NESThread::InfoNES_MessageBox(char *buf)
{
    qDebug() << buf;
}


