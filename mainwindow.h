#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QThread>
#include <QFile>
#include <QAudioFormat>
#include <QAudioOutput>
#include "keysetting.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class NESThread : public QThread
{
    Q_OBJECT

public:
    explicit NESThread(QObject *parent = nullptr, void *buff = nullptr, QString pszFileName = "");
    ~NESThread();

    void setMute(bool mute);

    int InfoNES_OpenRom(const char *pszFileName);
    int InfoNES_ReadRom(void *buf, unsigned int len);
    void InfoNES_CloseRom(void);
    void InfoNES_Wait(uint32_t us);
    void InfoNES_LoadFrame(uint16_t *frame, uint32_t size);
    void InfoNES_PadState(uint32_t *pdwPad1, uint32_t *pdwPad2, uint32_t *pdwSystem);
    void InfoNES_SoundOutput(int samples, uint8_t *wave1, uint8_t *wave2, uint8_t *wave3,
                             uint8_t *wave4, uint8_t *wave5);
    void InfoNES_SoundClose(void);
    int InfoNES_SoundOpen(int samples_per_sync, int sample_rate);
    void InfoNES_SoundInit(void);
    void InfoNES_MessageBox(char *buf);
    uint16_t *workFrame;
    uint32_t pdwPad1 = 0;
    uint32_t pdwPad2 = 0;
    uint32_t pdwSystem = 0;
    QImage *qImg;

protected:
    void run();

private:
    QFile *file = nullptr;
    QByteArray *fileName = nullptr;
    QAudioOutput *audio = nullptr;
    QAudioFormat *audioFormat = nullptr;
    uchar *audio_buff = nullptr;
    QIODevice *audio_dev = nullptr;
    bool m_mute = false;
};

class DGENThread : public QThread
{
    Q_OBJECT

public:
    explicit DGENThread(QObject *parent = nullptr, void *buff = nullptr, QString pszFileName = "");
    ~DGENThread();

    void setMute(bool mute);

    int DGEN_OpenRom(const char *pszFileName);
    int DGEN_ReadRom(void *buf, unsigned int len);
    void DGEN_CloseRom(void);
    void DGEN_Wait(uint32_t us);
    void DGEN_LoadFrame(uint8_t *frame);
    void DGEN_PadState(uint32_t *pdwPad1, uint32_t *pdwPad2, uint32_t *pdwSystem);
    void DGEN_SoundOutput(int samples, int16_t *wave);
    void DGEN_SoundClose(void);
    int DGEN_SoundOpen(int samples_per_sync, int sample_rate);
    void DGEN_SoundInit(void);
    void DGEN_MessageBox(char *buf);
    uint16_t *workFrame;
    uint32_t pdwPad1 = 0;
    uint32_t pdwPad2 = 0;
    uint32_t pdwSystem = 0;
    QImage *qImg;

protected:
    void run();

private:
    QFile *file = nullptr;
    QByteArray *fileName = nullptr;
    QAudioOutput *audio = nullptr;
    QAudioFormat *audioFormat = nullptr;
    short *audio_buff = nullptr;
    QIODevice *audio_dev = nullptr;
    bool m_mute = false;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void timer_repaint();
    void sample_1_triggered();
    void sample_2_triggered();
    void sample_3_triggered();
    void sample_4_triggered();
    void sample_5_triggered();
    void open_triggered();
    void close_triggered();
    void mute_triggered();
    void key_setting_triggered();
    void about_triggered();
    void about_qt_triggered();

private:
    Ui::MainWindow *ui;
    QImage *qImg;
    uchar *buff;
    NESThread *nesThread = nullptr;
    DGENThread *dgenThread = nullptr;
    QTimer *timer;
    KeySetting *key_setting = nullptr;
    void start_nesThread(QString file_name);
    void start_dgenThread(QString file_name);
};

#endif // MAINWINDOW_H
