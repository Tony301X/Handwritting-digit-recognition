#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HandWriting.h"

#include <QMainWindow>
#include <QToolButton>
#include <QLabel>
#include <QComboBox>              //下拉列表框
#include <QSpinBox>               //自选盒
#include <QToolBar>
#include <QColorDialog>
#include <QWidget>
#include <QPainter>
#include <QList>
#include <QMouseEvent>
#include <QImage>
#include <QDebug>
#include <QTextEdit>
#include <QMessageBox>
#include <qtimer.h>
#include "Algo.h"

#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>  

class HandWriting : public QMainWindow
{
    Q_OBJECT

public:
    HandWriting(QWidget *parent = Q_NULLPTR);
    ~HandWriting();
    void print(QString s);

    Ui::HandWritingClass ui;

    Algo algo;

    QVector<QLineF> m_lineFList;
    QPointF m_startPointF;
    QPointF m_endPointF;
    QPointF offset;
    bool m_bValidDraw;  //有效绘画
    bool m_bStart;      //True 开始绘制 False 结束绘制

    //opencv摄像头句柄
    VideoCapture *cam;
    //储存图像
    Mat input_img;
    //摄像头刷新定时器
    QTimer* timer;
   
    bool eventFilter(QObject *watched, QEvent * event);
    //void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str);

    //void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void drawBackGround(QPainter&);         //绘制背景
    void drawUserDraw(QPainter&);           //用户绘制
    
    void Painter();

public slots:
    //清除按钮
	void Clear_Btn_click();
    //获取特征按钮
    void Get_Feature_Btn_click();
    //计算分数按钮
    void Cal_Score_Btn_click();
    void about_msg();

    int get_frame();
};
