#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QThread>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class NESThread : public QThread
{
    Q_OBJECT

public:
    explicit NESThread(QObject *parent = nullptr,uint16_t* buff = nullptr,QString pszFileName = "");
    ~NESThread();
    int InfoNES_OpenRom(const char *pszFileName);
    int InfoNES_ReadRom(void *buf, unsigned int len);
    void InfoNES_CloseRom(void);
    void InfoNES_Wait(void);
    void InfoNES_LoadFrame(void);
    void InfoNES_PadState(uint32_t *pdwPad1, uint32_t *pdwPad2, uint32_t *pdwSystem);
    void InfoNES_SoundOutput(int samples, uint8_t *wave1, uint8_t *wave2, uint8_t *wave3,
                             uint8_t *wave4, uint8_t *wave5);
    void InfoNES_SoundClose(void);
    int InfoNES_SoundOpen(int samples_per_sync, int sample_rate);
    void InfoNES_SoundInit(void);
    void InfoNES_MessageBox(char *buf);
    uint16_t* workFrame;
    uint32_t pdwPad1 = 0;
    uint32_t pdwPad2 = 0;
    uint32_t pdwSystem = 0;

protected:
    void run();

private:
    QFile *file = nullptr;
    QByteArray *fileName = nullptr;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

private slots:
    void timer_repaint();
    void open_triggered();
    void close_triggered();
    void about_triggered();
    void sample_1_triggered();
    void sample_2_triggered();
    void sample_3_triggered();
    void sample_4_triggered();

private:
    Ui::MainWindow *ui;
    QImage *qImg;
    uint16_t* buff;
    NESThread *nesThread = nullptr;
    QTimer *timer;
};


#endif // MAINWINDOW_H
