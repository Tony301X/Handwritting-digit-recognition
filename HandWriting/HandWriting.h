#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HandWriting.h"

#include <QMainWindow>
#include <QToolButton>
#include <QLabel>
#include <QComboBox>              //�����б��
#include <QSpinBox>               //��ѡ��
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
#include "Algo.h"

class HandWriting : public QMainWindow
{
    Q_OBJECT

public:
    HandWriting(QWidget *parent = Q_NULLPTR);
    void print(QString s);

    Ui::HandWritingClass ui;

    Algo algo;

    QVector<QLineF> m_lineFList;
    QPointF m_startPointF;
    QPointF m_endPointF;
    QPointF offset;
    bool m_bValidDraw;  //��Ч�滭
    bool m_bStart;      //True ��ʼ���� False ��������

    bool eventFilter(QObject *watched, QEvent * event);
    //void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str);

    //void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void drawBackGround(QPainter&);         //���Ʊ���
    void drawUserDraw(QPainter&);           //�û�����
    
    void Painter();
public slots:
    //�����ť
	void Clear_Btn_click();
    //��ȡ������ť
    void Get_Feature_Btn_click();
    //���������ť
    void Cal_Score_Btn_click();
    void about_msg();
};
