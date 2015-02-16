#ifndef BadTest2_H
#define BadTest2_H

#include <QMainWindow>
#include <QtGui>
#include <QtWidgets>
#include <QtCore>
#include <QRgb>
#include <QColor>
#include "bass.h"

namespace Ui {
class BadTest2;
}

class BadTest2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit BadTest2(QWidget *parent = 0);
    //绘制一场画面
    inline void buttonDraw(int frame);
    inline void buttonDraw();
    //清除一块指定的矩形区域
    inline void clearRect(int *array, int x, int y, int width, int height);
    //排列按钮
    void arrangement();
    ~BadTest2();

private:
    Ui::BadTest2 *ui;
    QPushButton *button[80*60];
    QTimer *timer;
    int curTime;
    char *frames;
    HSTREAM nowPlay;
    int mode;//选择所使用的算法
public slots:
    void timerUpdate();
protected:
    void paintEvent(QPaintEvent *);

private slots:
    void on_algorithm1_clicked();
    void on_algorithm2_clicked();
    void on_algorithm3_clicked();
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_playingProgress_valueChanged(int value);
    void on_algorithm0_clicked();
};

#endif // BadTest2_H
