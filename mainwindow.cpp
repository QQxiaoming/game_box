#include <QPixmap>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#define SOUND_NUM_FARME 2
#define MAX_WIDTH 320
#define MAX_HEIGHT 240

const QString MainWindow::VERSION = APP_VERSION;
const QString MainWindow::GIT_TAG =
#include <git_tag.inc>
;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QRect screen = QGuiApplication::screenAt(
        this->mapToGlobal(QPoint(this->width()/2,0)))->geometry();
    QRect size = this->geometry();
    this->move((screen.width() - size.width()) / 2,
                (screen.height() - size.height()) / 2);

    key_setting = new KeySetting();
    timer = new QTimer(this);
    buff = new uchar[MAX_WIDTH * MAX_HEIGHT * 2];
    qImg = new QImage(buff, MAX_WIDTH, MAX_HEIGHT, QImage::Format_RGB555);
    memset(buff, 0x0, MAX_WIDTH * MAX_HEIGHT * 2);
    this->setWindowTitle("GameBox");
    timer->start(17);

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timer_repaint()));
    QObject::connect(ui->action_sample_1, SIGNAL(triggered()), this, SLOT(sample_1_triggered()));
    QObject::connect(ui->action_sample_2, SIGNAL(triggered()), this, SLOT(sample_2_triggered()));
    QObject::connect(ui->action_sample_3, SIGNAL(triggered()), this, SLOT(sample_3_triggered()));
    QObject::connect(ui->action_sample_4, SIGNAL(triggered()), this, SLOT(sample_4_triggered()));
    QObject::connect(ui->action_sample_5, SIGNAL(triggered()), this, SLOT(sample_5_triggered()));
    QObject::connect(ui->action_open, SIGNAL(triggered()), this, SLOT(open_triggered()));
    QObject::connect(ui->action_close, SIGNAL(triggered()), this, SLOT(close_triggered()));
    QObject::connect(ui->action_mute, SIGNAL(triggered()), this, SLOT(mute_triggered()));
    QObject::connect(ui->action_key_setting, SIGNAL(triggered()), this, SLOT(key_setting_triggered()));
    QObject::connect(ui->action_about, SIGNAL(triggered()), this, SLOT(about_triggered()));
    QObject::connect(ui->action_about_qt, SIGNAL(triggered()), this, SLOT(about_qt_triggered()));
}

MainWindow::~MainWindow() {
    if (nesThread != nullptr) {
        delete nesThread;
        nesThread = nullptr;
    }
    if (dgenThread != nullptr) {
        delete dgenThread;
        dgenThread = nullptr;
    }
    delete qImg;
    delete[] buff;
    delete timer;
    delete key_setting;
    delete ui;
}

void MainWindow::start_nesThread(QString file_name) {
    close_triggered();
    QFileInfo fileinfo = QFileInfo(file_name);
    this->setWindowTitle(fileinfo.fileName());
    nesThread = new NESThread(this, buff, file_name);
    nesThread->setMute(ui->action_mute->isChecked());
    nesThread->start();
}

void MainWindow::start_dgenThread(QString file_name) {
    close_triggered();
    QFileInfo fileinfo = QFileInfo(file_name);
    this->setWindowTitle(fileinfo.fileName());
    dgenThread = new DGENThread(this, buff, file_name);
    dgenThread->setMute(ui->action_mute->isChecked());
    dgenThread->start();
}

void MainWindow::sample_1_triggered() {
    QString file_name = ":/game/games/SuperMario.nes";
    start_nesThread(file_name);
}

void MainWindow::sample_2_triggered() {
    QString file_name = ":/game/games/CatAndMouse.nes";
    start_nesThread(file_name);
}

void MainWindow::sample_3_triggered() {
    QString file_name = ":/game/games/Tanks.nes";
    start_nesThread(file_name);
}

void MainWindow::sample_4_triggered() {
    QString file_name = ":/game/games/MacrossSeries.nes";
    start_nesThread(file_name);
}

void MainWindow::sample_5_triggered() {
    QString file_name = ":/game/games/tiger_hunter_hero_novel_(chinese).smd";
    start_dgenThread(file_name);
}

void MainWindow::open_triggered() {
    QString file_name = QFileDialog::getOpenFileName(
        this, "选择文件", "", "NES ROM(*.nes);;MD ROM(*.smd);;Bin file(*.bin);;All file(*)");
    if (file_name.isEmpty()) {
        return;
    }

    QFileInfo fileinfo = QFileInfo(file_name);
    QString file_suffix = fileinfo.suffix();
    if (file_suffix == "nes") {
        start_nesThread(file_name);
    } else if (file_suffix == "smd") {
        start_dgenThread(file_name);
    } else {
        QMessageBox messageBox(
            QMessageBox::Question, "提示", "请选择文件格式:", QMessageBox::NoButton, this);
        messageBox.addButton("NES", QMessageBox::ActionRole);
        messageBox.addButton("MD", QMessageBox::ActionRole);
        messageBox.addButton("取消", QMessageBox::ActionRole);
        int format = messageBox.exec();
        if (format == 0) {
            start_nesThread(file_name);
        } else if (format == 1) {
            start_dgenThread(file_name);
        }
    }
}

void MainWindow::close_triggered() {
    if (nesThread != nullptr) {
        delete nesThread;
        nesThread = nullptr;
    }
    if (dgenThread != nullptr) {
        delete dgenThread;
        dgenThread = nullptr;
    }
    memset(buff, 0x0, MAX_WIDTH * MAX_HEIGHT * 2);
    this->setWindowTitle("GameBox");
}

void MainWindow::mute_triggered() {
    static bool mute = true;
    ui->action_mute->setChecked(mute);
    if (nesThread != nullptr) {
        nesThread->setMute(mute);
    }
    if (dgenThread != nullptr) {
        dgenThread->setMute(mute);
    }
    mute = !mute;
}

void MainWindow::key_setting_triggered() {
    key_setting->show();
}

void MainWindow::about_triggered() {
    QMessageBox::about(this, "关于GameBox",
        "版本 \n " + VERSION + "\n"
        "提交 \n " + GIT_TAG + "\n"
        "作者 \n qiaoqm@aliyun.com");
}

void MainWindow::about_qt_triggered() {
    QMessageBox::aboutQt(this);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter;
    QImage *qImg = this->qImg;
    if (nesThread != nullptr) {
        qImg = nesThread->qImg;
    } else if (dgenThread != nullptr) {
        qImg = dgenThread->qImg;
    }

    painter.begin(this);
    painter.drawPixmap(QPoint(0, 25), 
                QPixmap::fromImage(qImg->scaled(this->size() - QSize(0, 25))));
    painter.end();

    Q_UNUSED(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_F1) {
        if (nesThread != nullptr) {
            QMessageBox::about(
                this, "About Emulators", "当前模拟器版本：\n  " + nesThread->libVersion);
        } else if (dgenThread != nullptr) {
            QMessageBox::about(
                this, "About Emulators", "当前模拟器版本：\n  " + dgenThread->libVersion);
        }
    } else if (nesThread != nullptr) {
        nesThread->processQtKeyEvent(static_cast<Qt::Key>(event->key()),true);
    } else if (dgenThread != nullptr) {
        dgenThread->processQtKeyEvent(static_cast<Qt::Key>(event->key()),true);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (nesThread != nullptr) {
        nesThread->processQtKeyEvent(static_cast<Qt::Key>(event->key()),false);
    } else if (dgenThread != nullptr) {
        dgenThread->processQtKeyEvent(static_cast<Qt::Key>(event->key()),false);
    }
}

void MainWindow::timer_repaint() {
    this->repaint();
}

NESThread::NESThread(QObject *parent, void *buff, QString pszFileName) 
    : QThread(parent) {
    qImg = new QImage(static_cast<uchar *>(buff), 256, 240, QImage::Format_RGB555);
    workFrame = static_cast<uint16_t *>(buff);
    fileName = new QByteArray(pszFileName.toUtf8().data(), pszFileName.toUtf8().size());
}

NESThread::~NESThread() {
    this->pdwSystem = 1;
    requestInterruption();
    quit();
    wait();
    delete fileName;
    delete qImg;
}

void NESThread::run() {
    extern void InfoNES_start(NESThread * nesThread, const char *pszFileName);
    InfoNES_start(this, fileName->data());
}

void NESThread::processQtKeyEvent(Qt::Key key,bool press) {
    const QList<QPair<Qt::Key, uint16_t>> nesKeyMap = {
        {Qt::Key_Period,0},  {Qt::Key_Slash,1},
        {Qt::Key_Control,2}, {Qt::Key_Return,3},
        {Qt::Key_W,4},       {Qt::Key_S,5},
        {Qt::Key_A,6},       {Qt::Key_D,7},
        {Qt::Key_1,0<<8},    {Qt::Key_2,1<<8},
        {Qt::Key_Plus,2<<8}, {Qt::Key_Enter,3<<8},
        {Qt::Key_Up,4<<8},   {Qt::Key_Down,5<<8},
        {Qt::Key_Left,6<<8}, {Qt::Key_Right,7<<8},
    };
    uint8_t keyVale1 = 0xff;
    uint8_t keyVale2 = 0xff;
    QList<QPair<Qt::Key, uint16_t>>::const_iterator it = nesKeyMap.begin();
    while (it != nesKeyMap.end()) {
        if(it->first == key) {
            uint16_t value = it->second;
            keyVale1 = value&0xff;
            keyVale2 = (value>>8)&0xff;
            break;
        }
        it++;
    }
    if(keyVale1 != 0xff) {
        if(press) {
            this->pdwPad1 |= (0x1UL << keyVale1);
        } else {
            this->pdwPad1 &= ~(0x1UL << keyVale1);
        }
    }
    if(keyVale2 != 0xff) {
        if(press) {
            this->pdwPad2 |= (0x1UL << keyVale2);
        } else {
            this->pdwPad2 &= ~(0x1UL << keyVale2);
        }
    }
}

void NESThread::setMute(bool mute) {
    m_mute = mute;
}

int NESThread::InfoNES_OpenRom(const char *pszFileName) {
    if (file != nullptr) {
        if (file->isOpen()) {
            return static_cast<int>(file->size());
        }
    }
    file = new QFile(pszFileName);
    if (file->open(QFile::ReadOnly)) {
        return static_cast<int>(file->size());
    } else {
        return -1;
    }
}

int NESThread::InfoNES_ReadRom(void *buf, unsigned int len) {
    return static_cast<int>(file->read(static_cast<char *>(buf), len));
}

void NESThread::InfoNES_CloseRom(void) {
    if (file->isOpen()) {
        file->close();
        delete file;
        file = nullptr;
    }
}

void NESThread::InfoNES_Wait(uint32_t us) {
    this->usleep(us);
}

void NESThread::InfoNES_LoadFrame(uint16_t *frame, uint32_t size) {
    memcpy(workFrame, frame, size);
}

void NESThread::InfoNES_PadState(uint32_t *pdwPad1, uint32_t *pdwPad2, uint32_t *pdwSystem) {
    *pdwPad1 = this->pdwPad1;
    *pdwPad2 = this->pdwPad2;
    *pdwSystem = this->pdwSystem;
}

void NESThread::InfoNES_SoundInit(void) {
    audioFormat = new QAudioFormat();
    audioFormat->setChannelCount(1);
    audioFormat->setSampleFormat(QAudioFormat::UInt8);
}

int NESThread::InfoNES_SoundOpen(int samples_per_sync, int sample_rate) {
#if 1 //TODO: fix audio
    delete audioFormat;
    audioFormat = nullptr;
    return -1;
#else
    audioFormat->setSampleRate(sample_rate);
    QAudioDevice info(QMediaDevices::defaultAudioOutput()); //选择默认输出设备
    if (!info.isFormatSupported(*audioFormat)) {
        delete audioFormat;
        audioFormat = nullptr;
        return -1;
    }

    audio = new QAudioSink(*audioFormat, nullptr);
    audio_buff = new uchar[samples_per_sync * SOUND_NUM_FARME];
    memset(audio_buff, 0x0, static_cast<size_t>(samples_per_sync * SOUND_NUM_FARME));
    audio->setBufferSize(samples_per_sync * 10);
    audio_dev = audio->start();
    return 0;
#endif
}

void NESThread::InfoNES_SoundClose(void) {
    if(!audio) {
        return;
    }
    delete audio;
    delete audioFormat;
    delete[] audio_buff;
    audio = nullptr;
    audioFormat = nullptr;
    audio_buff = nullptr;
}

void NESThread::InfoNES_SoundOutput(int samples, uint8_t *wave1, uint8_t *wave2, uint8_t *wave3,
                                    uint8_t *wave4, uint8_t *wave5) {
    if(!audio) {
        return;
    }
    static int index = 0;
    for (int i = 0; i < samples; i++) {
        uint32_t wav = (static_cast<uint32_t>(wave1[i]) + //TODO: envelope generator和sweep unit未实现
                        static_cast<uint32_t>(wave2[i]) + //TODO: envelope generator和sweep unit未实现
                        //static_cast<uint32_t>(wave3[i]) + //TODO: 音频输出不正确
                        static_cast<uint32_t>(wave4[i]) + //TODO: envelope generator未实现
                        //static_cast<uint32_t>(wave5[i]) + //TODO: 音频输出不正确
                        0) / 5UL;
        Q_UNUSED(wave1);
        Q_UNUSED(wave2);
        Q_UNUSED(wave3);
        Q_UNUSED(wave4);
        Q_UNUSED(wave5);
        if (m_mute) {
            audio_buff[i + index * samples] = 0;
        } else {
            audio_buff[i + index * samples] = static_cast<uchar>(wav);
        }
    }
    if (index < SOUND_NUM_FARME - 1) {
        index++;
    } else {
        int len = 0;
        forever {
            void *temp = static_cast<void *>(audio_buff + len);
            int n = static_cast<int>(audio_dev->write(
                                static_cast<char *>(temp),
                                static_cast<qint64>(samples * SOUND_NUM_FARME - len)));
            if (n == -1) {
                break;
            } else {
                len += n;
                if (samples * SOUND_NUM_FARME == len)
                    break;
            }
        }
        index = 0;
    }
}

void NESThread::InfoNES_MessageBox(char *buf) {
    qDebug() << buf;
}

DGENThread::DGENThread(QObject *parent, void *buff, QString pszFileName) 
    : QThread(parent) {
    qImg = new QImage(static_cast<uchar *>(buff), 304, 224, QImage::Format_RGB555);
    workFrame = static_cast<uint16_t *>(buff);
    fileName = new QByteArray(pszFileName.toUtf8().data(), pszFileName.toUtf8().size());
}

DGENThread::~DGENThread() {
    this->pdwSystem = 1;
    requestInterruption();
    quit();
    wait();
    delete fileName;
    delete qImg;
}

void DGENThread::run() {
    extern void DGEN_start(DGENThread * dgenThread, const char *pszFileName);
    DGEN_start(this, fileName->data());
}

void DGENThread::processQtKeyEvent(Qt::Key key,bool press) {
    const QList<QPair<Qt::Key, uint16_t>> dgenKeyMap = {
        {Qt::Key_W,0},            {Qt::Key_S,1},
        {Qt::Key_A,2},            {Qt::Key_D,3},
        {Qt::Key_Period,4},       {Qt::Key_Slash,5},
        {Qt::Key_Comma,12},       {Qt::Key_Return,13},
        {Qt::Key_Apostrophe,16},  {Qt::Key_Semicolon,17},
        {Qt::Key_L,18},           {Qt::Key_Control,19},
        {Qt::Key_Up,0<<8},       {Qt::Key_Down,1<<8},
        {Qt::Key_Left,2<<8},     {Qt::Key_Right,3<<8},
        {Qt::Key_3,4<<8},        {Qt::Key_2,5<<8},
        {Qt::Key_1,12<<8},       {Qt::Key_Enter,13<<8},
        {Qt::Key_6,16<<8},       {Qt::Key_5,17<<8},
        {Qt::Key_4,18<<8},       {Qt::Key_Plus,19<<8},
    };
    uint8_t keyVale1 = 0xff;
    uint8_t keyVale2 = 0xff;
    QList<QPair<Qt::Key, uint16_t>>::const_iterator it = dgenKeyMap.begin();
    while (it != dgenKeyMap.end()) {
        if(it->first == key) {
            uint16_t value = it->second;
            keyVale1 = value&0xff;
            keyVale2 = (value>>8)&0xff;
            break;
        }
        it++;
    }
    if(keyVale1 != 0xff) {
        if(!press) {
            this->pdwPad1 |= (0x1UL << keyVale1);
        } else {
            this->pdwPad1 &= ~(0x1UL << keyVale1);
        }
    }
    if(keyVale2 != 0xff) {
        if(!press) {
            this->pdwPad2 |= (0x1UL << keyVale2);
        } else {
            this->pdwPad2 &= ~(0x1UL << keyVale2);
        }
    }
}

void DGENThread::setMute(bool mute) {
    m_mute = mute;
}

int DGENThread::DGEN_OpenRom(const char *pszFileName) {
    if (file != nullptr) {
        if (file->isOpen()) {
            return static_cast<int>(file->size());
        }
    }
    file = new QFile(pszFileName);
    if (file->open(QFile::ReadOnly)) {
        return static_cast<int>(file->size());
    } else {
        return -1;
    }
}

int DGENThread::DGEN_ReadRom(void *buf, unsigned int len) {
    return static_cast<int>(file->read(static_cast<char *>(buf), len));
}

void DGENThread::DGEN_CloseRom(void) {
    if (file->isOpen()) {
        file->close();
        delete file;
        file = nullptr;
    }
}

void DGENThread::DGEN_Wait(uint32_t us) {
    this->usleep(us);
}

void DGENThread::DGEN_LoadFrame(uint8_t *frame) {
    for (int i = 0; i < 224; i++) {
        memcpy(workFrame + i * 304, frame + (i + 8) * 2 * 336 + (16) * 2, 304 * 2);
    }
}

void DGENThread::DGEN_PadState(uint32_t *pdwPad1, uint32_t *pdwPad2, uint32_t *pdwSystem) {
    *pdwPad1 = this->pdwPad1;
    *pdwPad2 = this->pdwPad2;
    *pdwSystem = this->pdwSystem;
}

void DGENThread::DGEN_SoundInit(void) {
    audioFormat = new QAudioFormat();
    audioFormat->setChannelCount(2);
    audioFormat->setSampleFormat(QAudioFormat::Int16);
}

int DGENThread::DGEN_SoundOpen(int samples_per_sync, int sample_rate) {
    audioFormat->setSampleRate(sample_rate);
    QAudioDevice info(QMediaDevices::defaultAudioOutput()); //选择默认输出设备
    if (!info.isFormatSupported(*audioFormat)) {
        delete audioFormat;
        audioFormat = nullptr;
        return -1;
    }

    audio = new QAudioSink(*audioFormat, nullptr);
    audio_buff = new short[2 * samples_per_sync * SOUND_NUM_FARME];
    memset(audio_buff, 0x0, static_cast<size_t>(2 * 2 * samples_per_sync * SOUND_NUM_FARME));
    audio->setBufferSize(2 * 2 * samples_per_sync * 10);
    audio_dev = audio->start();
    return 0;
}

void DGENThread::DGEN_SoundClose(void) {
    if(!audio) {
        return;
    }
    delete audio;
    delete audioFormat;
    delete[] audio_buff;
    audio = nullptr;
    audioFormat = nullptr;
    audio_buff = nullptr;
}

void DGENThread::DGEN_SoundOutput(int samples, int16_t *wave) {
    if(!audio) {
        return;
    }
    static int index = 0;
    for (int i = 0; i < 2 * samples; i++) {
        if (m_mute) {
            audio_buff[i + index * 2 * samples] = 0;
        } else {
            audio_buff[i + index * 2 * samples] = wave[i];
        }
    }
    if (index < SOUND_NUM_FARME - 1) {
        index++;
    } else {
        int len = 0;
        forever {
            void *temp = static_cast<void *>(audio_buff);
            char *temp1 = static_cast<char *>(temp);
            int n = static_cast<int>(audio_dev->write(
                            temp1 + len, 
                            static_cast<qint64>(2 * 2 * samples * SOUND_NUM_FARME - len)));
            if (n == -1) {
                break;
            } else {
                len += n;
                if (2 * 2 * samples * SOUND_NUM_FARME == len)
                    break;
            }
        }
        index = 0;
    }
}

void DGENThread::DGEN_MessageBox(char *buf) {
    qDebug() << buf;
}
