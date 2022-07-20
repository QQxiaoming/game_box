#ifndef KEYSETTING_H
#define KEYSETTING_H

#include <QWidget>
#include <QImage>
#include <QEvent>

namespace Ui {
    class KeySetting;
}

class KeySetting : public QWidget {
    Q_OBJECT

public:
    explicit KeySetting(QWidget *parent = nullptr);
    ~KeySetting();

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    Ui::KeySetting *ui;
    QImage *qImg;
};

#endif // KEYSETTING_H
