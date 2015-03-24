#include "badtest2.h"
#include "ui_badtest2.h"

/* 0~5476 共5477帧 25FPS */

BadTest2::BadTest2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BadTest2)
{
    ui->setupUi(this);
    mode = 3;
    //初始化一堆按钮
    for (int i=0;i<(80*60);i++)
    {
        button[i] = new QPushButton(this);
        button[i]->setVisible(false);
    }

    //初始化所有画面
    frames = new char[5477*80*60];

    //打表
    for (int f=0;f<5477;f++)
    {

        QImage temp(QString(":BA/BadApple/") + QString::number(f+1) + QString(".bmp"));
        for (int y=0;y<60;y++)
        {
            for (int x=0;x<80;x++)
            {
                if (QColor(temp.pixel(x,y)).lightness() > 128)
                    frames[f*4800+y*80+x] = 1;
                else
                    frames[f*4800+y*80+x] = 0;
            }
        }
    }

    curTime = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->setInterval(30);

    BASS_Init(-1, 44100, 0, 0, NULL);
    nowPlay = BASS_StreamCreateFile(false,QString("BadApple.mp3").toStdWString().c_str(),0,0,BASS_UNICODE | BASS_STREAM_PRESCAN);
    timer->start();
}

void BadTest2::arrangement()
{
    //重排按钮
    //行优先方式
    /* 像这样（对应一维数组下标）
    0,1,2,3,4,5,6,7,8,9……
    80,82,83,84……
    ……
    ……4798,4799
    */
    for (int i = 0; i < 4800; i++)
    {
        button[i]->setGeometry(i%80*10, (i/80)%60*10, 10, 10);
    }
}

inline void BadTest2::buttonDraw()
{
    //绘制当期帧
    int frame = (int)(BASS_ChannelBytes2Seconds(nowPlay,BASS_ChannelGetPosition(nowPlay, BASS_POS_BYTE))*25.0);
    if (frame > 5476 || frame < 0)
    {
        frame = 0;
    }
    buttonDraw(frame);
}

inline void BadTest2::buttonDraw(int frame)
{
    int curFrame[80*60];
    //copy当期帧画面
    for (int i=0; i<4800; i++)
    {
        curFrame[i] = frames[frame*4800+i];
    }

    if (mode == 0)
    {
        //原始方式
        int totalOfButton = 0;
        for (int y = 0; y < 60; y++)
        {
            for (int x = 0; x < 80; x++)
            {
                if (curFrame[y*80+x])
                {
                    button[y*80+x]->setVisible(true);
                    totalOfButton++;
                }
                else
                {
                    button[y*80+x]->setVisible(false);
                }
            }
        }
        ui->label->setText(QString("按钮数：") + QString::number(totalOfButton));
    }
    else
    {
        //装逼方式

        //当前操作的按钮编号
        int buttonID = 0;

        switch (mode) {
        case 1:
            for (int y = 0; y < 60; y++)
            {
                for (int x = 0; x < 80; x++)
                {
                    if (curFrame[y*80+x])
                    {
                        //此处有像素点
                        int lenght = 1;
                        for (int i = x+1; i < 80; i++)
                        {
                            if (curFrame[y*80+i])
                                lenght++;
                            else
                                break;
                        }
                        //clearRect(curFrame, x, y, lenght, 1);
                        button[buttonID]->setGeometry(x*10,y*10,10*lenght,10);
                        button[buttonID++]->setVisible(true);
                        x+=lenght-1;
                    }
                }
            }
            break;
        case 2:
            //搜寻正方形
            for (int y = 0; y < 60; y++)
            {
                for (int x = 0; x < 80; x++)
                {
                    //求解出的边长
                    int side = 1;

                    //搜索到画面非空白内容
                    if (curFrame[y*80+x])
                    {
                        //求当前允许最大边长
                        int maxSide = (80-x)<(60-y)?(80-x):(60-y);//最小值1（在边框上），最大值60
                        //搜索最大正方形
                        for (int sideAdd = 1; sideAdd < maxSide; sideAdd++)
                        {
                            bool testXOK = true;
                            //测试横向底边
                            for (int xLen = 0; xLen <= sideAdd; xLen++)
                            {
                                //发现非1像素
                                if(curFrame[(y+sideAdd)*80+(x+xLen)] == 0)
                                {
                                    testXOK = false;
                                    break;//停止横向搜索
                                }
                            }
                            //如果横向测试通过，测试纵向
                            bool testYOK = true;
                            if (testXOK)
                            {
                                for (int yLen = 0; yLen <= sideAdd; yLen++)
                                {
                                    //发现非1像素
                                    if(curFrame[(y+yLen)*80+(x+sideAdd)] == 0)
                                    {
                                        testYOK = false;
                                        break;//停止纵向搜索
                                    }
                                }
                            }
                            if (testXOK && testYOK)
                            {
                                side++;//增加边长
                            }
                            else
                            {
                                break;//无更大正方形，结束算法
                            }
                        }

                        clearRect(curFrame, x, y, side, side);
                        //操作界面控件
                        button[buttonID]->setGeometry(x*10,y*10,side*10,side*10);
                        button[buttonID++]->setVisible(true);
                    }
                }
            }
            break;
        case 3:
            //算法2
            for (int side = 60; side > 0; side--)
            {
                //搜索全画面中存在的边长为side的正方形 最大60 最小1
                for (int y = 0; y <= 60-side; y++)
                {
                    for (int x = 0; x <= 80-side; x++)
                    {
                        //遍历所有可搜索区域
                        bool isOk = true;//记录该区域是否存在正方形
                        //测试所有像素
                        for (int i = 0; i < side; i++)
                        {
                            for (int j = 0; j < side; j++)
                            {
                                if (curFrame[(y+i)*80+(x+j)] == 0)
                                {
                                    //测试失败
                                    isOk = false;
                                    goto FUCK;//跳出两层循环
                                }
                            }
                        }
                        FUCK:
                        if (isOk)
                        {
                            clearRect(curFrame, x, y, side, side);
                            //操作界面控件
                            button[buttonID]->setGeometry(x*10,y*10,side*10,side*10);
                            button[buttonID++]->setVisible(true);
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
        ui->label->setText(QString("按钮数：") + QString::number(buttonID));
        //隐藏剩余按钮
        for (int i=buttonID; i<(80*60); i++)
        {
            button[i]->setVisible(false);
        }
    }
}

//清空一块区域
inline void BadTest2::clearRect(int *array, int x, int y, int width, int height)
{
    for (int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            array[(y+j)*80+(x+i)] = 0;
        }
    }
}

void BadTest2::timerUpdate()
{
    //计算当前帧
    int newTime = 0;
    //if (BASS_ChannelIsActive(nowPlay) == BASS_ACTIVE_PLAYING)
    newTime = (int)(BASS_ChannelBytes2Seconds(nowPlay,BASS_ChannelGetPosition(nowPlay, BASS_POS_BYTE))*25.0);

    //同一帧

    if (newTime == curTime)
        return;

    curTime = newTime;

    if (curTime > 5476 || curTime < 0)
    {
        curTime = 0;
        //timer->stop();
    }

    //---------
    buttonDraw(curTime);
    ui->playingProgress->setValue((int)(BASS_ChannelBytes2Seconds(nowPlay,BASS_ChannelGetPosition(nowPlay, BASS_POS_BYTE))*25.0));

}

void BadTest2::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    painter.setBrush(Qt::SolidPattern);
    painter.drawRect(0,0,800,600);
    //painter.setPen(QColor(Qt::white));
    //painter.setBrush(QColor(Qt::white));
}

BadTest2::~BadTest2()
{
    delete ui;
}

void BadTest2::on_algorithm0_clicked()
{
    arrangement();
    this->mode = 0;
    buttonDraw();
}

void BadTest2::on_algorithm1_clicked()
{
    this->mode = 1;
    buttonDraw();
}

void BadTest2::on_algorithm2_clicked()
{
    this->mode = 2;
    buttonDraw();
}

void BadTest2::on_algorithm3_clicked()
{
    this->mode = 3;
    buttonDraw();
}

void BadTest2::on_playButton_clicked()
{
    BASS_ChannelPlay(nowPlay, false);
}

void BadTest2::on_pauseButton_clicked()
{
    BASS_ChannelPause(nowPlay);
}

void BadTest2::on_playingProgress_valueChanged(int value)
{
    //换算到帧
    int pos = (int)(BASS_ChannelBytes2Seconds(nowPlay,BASS_ChannelGetPosition(nowPlay, BASS_POS_BYTE))*25.0);
    if (qAbs(pos - value) > 4)
    {
        //帧换算到位置
        BASS_ChannelSetPosition(nowPlay, value*BASS_ChannelGetLength(nowPlay, BASS_POS_BYTE)/5476, BASS_POS_BYTE);
    }
}
