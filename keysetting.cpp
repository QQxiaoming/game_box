#include "keysetting.h"
#include "ui_keysetting.h"
#include <QPainter>
#include <QKeyEvent>

KeySetting::KeySetting(QWidget *parent) 
    : QWidget(parent), ui(new Ui::KeySetting)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle("按键设置");
    qImg = new QImage(":/img/img/key.png");
}

KeySetting::~KeySetting()
{
    delete qImg;
    delete ui;
}

void KeySetting::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.drawPixmap(QPoint(0, 0), QPixmap::fromImage(qImg->scaled(this->size() - QSize(0, this->size().height() / 2))));
    painter.end();

    Q_UNUSED(event);
}

void KeySetting::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Period:
    {
        ui->PushButtonA->setEnabled(false);
        break;
    }
    case Qt::Key_Slash:
    {
        ui->PushButtonB->setEnabled(false);
        break;
    }
    case Qt::Key_Control:
    {
        ui->PushButtonSelent->setEnabled(false);
        break;
    }
    case Qt::Key_Return:
    {
        ui->PushButtonEnter->setEnabled(false);
        break;
    }
    case Qt::Key_W:
    {
        ui->PushButtonUp->setEnabled(false);
        break;
    }
    case Qt::Key_S:
    {
        ui->PushButtonDown->setEnabled(false);
        break;
    }
    case Qt::Key_A:
    {
        ui->PushButtonLeft->setEnabled(false);
        break;
    }
    case Qt::Key_D:
    {
        ui->PushButtonRight->setEnabled(false);
        break;
    }
    }
}

void KeySetting::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Period:
    {
        ui->PushButtonA->setEnabled(true);
        break;
    }
    case Qt::Key_Slash:
    {
        ui->PushButtonB->setEnabled(true);
        break;
    }
    case Qt::Key_Control:
    {
        ui->PushButtonSelent->setEnabled(true);
        break;
    }
    case Qt::Key_Return:
    {
        ui->PushButtonEnter->setEnabled(true);
        break;
    }
    case Qt::Key_W:
    {
        ui->PushButtonUp->setEnabled(true);
        break;
    }
    case Qt::Key_S:
    {
        ui->PushButtonDown->setEnabled(true);
        break;
    }
    case Qt::Key_A:
    {
        ui->PushButtonLeft->setEnabled(true);
        break;
    }
    case Qt::Key_D:
    {
        ui->PushButtonRight->setEnabled(true);
        break;
    }
    }
}
