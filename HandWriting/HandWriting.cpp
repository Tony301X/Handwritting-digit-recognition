#include "HandWriting.h"

void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    QString txt = str;
    QFile outFile("debug.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

HandWriting::HandWriting(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    print("UI start");
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // ��ֹ��󻯰�ť | unable the max button of the main window
    setFixedSize(this->width(), this->height());                     // ��ֹ�϶����ڴ�С | unable changing the window size
    connect(ui.about, SIGNAL(triggered()), this, SLOT(about_msg()));
    m_startPointF = m_endPointF = QPoint(0, 0);
    offset = QPointF(-10, -80);
    ui.input->installEventFilter(this);
    //�ض���qdebug | redirect the qdebug to file
    qInstallMessageHandler(customMessageHandler);

}
/*print�������� | print fun definition*/ 
void HandWriting::print(QString s)
{
    ui.debug->appendPlainText(s);
}
/*��д��ʵ�� | handwriting*/
//�¼������� | A event filter to catch the paint event to input widget
bool HandWriting::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui.input && event->type() == QEvent::Paint)
    {
        Painter();
    }
    return QWidget::eventFilter(watched, event);
}

void HandWriting::Painter()
{

    QPainter painter(ui.input);
    if (m_bStart)
    {
        drawBackGround(painter);

        m_bStart = false;
    }
    else
    {

        drawUserDraw(painter);
    }
}
void HandWriting::mousePressEvent(QMouseEvent *event)
{
    m_bValidDraw = event->button() == Qt::LeftButton;
    
    if (m_bValidDraw)
        m_startPointF = event->pos() + offset;
    //m_lineFList.clear();
}
void HandWriting::mouseReleaseEvent(QMouseEvent *event)
{

    if (event->button() == Qt::RightButton)
    {
        m_bStart = true;
        update();
    }
}
void HandWriting::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bValidDraw)
    {
        m_endPointF = event->pos() + offset;

        m_lineFList << QLineF(m_startPointF, m_endPointF);
        m_startPointF = m_endPointF;
        update();
    }

}
void HandWriting::drawBackGround(QPainter&painter)
{
    painter.save();
    QPen pen; //���� | pen
    pen.setColor(Qt::white);
    QBrush brush(Qt::white); //��ˢ | brush
    painter.setPen(pen); //��ӻ��� | add pen to painter
    painter.setBrush(brush); //��ӻ�ˢ | add brush to painter
    painter.drawRect(geometry()); //���ƾ��� | draw rect
    painter.restore();
}
void HandWriting::drawUserDraw(QPainter &userPaint)
{

    userPaint.save();
    QPen pen; //���� | pen
    pen.setColor(Qt::black);
    pen.setWidth(2);    //���û��� | set the width of the pen
    QBrush brush(Qt::black); //��ˢ | brush
    userPaint.setPen(pen);
    userPaint.setBrush(brush);
    userPaint.drawLines(m_lineFList);
    userPaint.restore();
}

/*��ť�ۺ��� the slot of buttions*/
void HandWriting::Clear_Btn_click()
{
    print("Clear_Btn_click");
    m_lineFList.clear();
    update();
}
// qimageתmat | transform the qimage to mat
Mat QImage2Mat(const QImage& qimage)
{
    Mat mat = Mat(qimage.height(), qimage.width(), CV_8UC4, (uchar*)qimage.bits(), qimage.bytesPerLine());
    Mat mat2 = Mat(mat.rows, mat.cols, CV_8UC3);
    int from_to[] = { 0,0, 1,1, 2,2 };
    mixChannels(&mat, 1, &mat2, 1, from_to, 3);
    return mat2;
}
void HandWriting::Get_Feature_Btn_click()
{
    print("-------------------------------------");
    print("Get_Feature_Btn_click");
    QPixmap pix = QPixmap::grabWidget(ui.input);
    //pix.save("1.jpg");
    //Mat src = cv::imread("1.jpg");
    QImage img = pix.toImage();
    Mat src = QImage2Mat(img);
    vector<int>feature = algo.get_feature(src);
    if (feature.size() != 0)
    {
        // debug���feature vector | debug to print the feature vector
        QString tmp;
        for (int col = 0; col < FEATURE_DIM; col++)
        {
            tmp += QString::number(feature[col]);
            tmp += "  ";
        }
        print(tmp);
    }
    else
        print("input error");
}

void HandWriting::Cal_Score_Btn_click()
{
    print("----------------------------------------------------");
    print("Cal_Score_Btn_click");
    QPixmap pix = QPixmap::grabWidget(ui.input);
    //pix.save("2.jpg");
    //Mat src = cv::imread("2.jpg");
    QImage img = pix.toImage();
    Mat src = QImage2Mat(img);
    vector<int>feature = algo.get_feature(src);
    if (feature.size() != 0)
    {
        //debug���feature vector | debug to print the feature vector
        QString tmp;
        tmp += QStringLiteral("��ǰ����Ϊ��"); // | tmp += QStringLiteral("Current feature is :");
        for (int col = 0; col < FEATURE_DIM; col++)
        {
            tmp += QString::number(feature[col]);
            tmp += "  ";
        }
        print(tmp);
        //��鷽����ťѡ�����
        int algo_status = 0;
        if (ui.Min_ER->isChecked())
            algo_status = 1;
        if (ui.Fisher->isChecked())
            algo_status = 2;
        //ִ�в�ͬ���㷨
        if (algo_status == 1)
        {
            print(QStringLiteral("ʹ����С�������㷨"));
            //�����
            vector<double>score = algo.min_error_bayes(feature);
            //4.2 debug���feature vector
            QString tmp2;
            tmp2 += QStringLiteral("�������Ϊ��"); // | tmp2 += QStringLiteral("Clasification Score is :");
            for (int col = 0; col < CATEGORY; col++)
            {
                tmp2 += QString::number(score[col]);
                tmp2 += "  ";
            }
            print(tmp2);
            //���������
            std::vector<double>::iterator biggest = std::max_element(std::begin(score), std::end(score));
            //std::cout << "Max element is " << *biggest << " at position " << std::distance(std::begin(score), biggest) << std::endl;
            QString tmp3;
            tmp3 += QStringLiteral("�б���Ϊ��"); // | tmp3 += QStringLiteral("Clasification is :");
            tmp3 += QString::number(std::distance(std::begin(score), biggest));
            tmp3 += QStringLiteral("���Ŷ�Ϊ��"); // | tmp3 += QStringLiteral("Probability is :");
            tmp3 += QString::number(*biggest);
            print(tmp3);
            ui.output_label->setText(tmp3);
        }
        else if (algo_status == 2)
        {
            print(QStringLiteral("ʹ��Fisher�㷨"));
            vector<float> score = algo.fisher(feature);
            QString tmp;
            tmp += QStringLiteral("�б���Ϊ:"); // | tmp3 += QStringLiteral("Probability is :");
            for (int i = 0; i < score.size(); i++)
            {
                tmp += QString::number(score[i]);
                tmp += "  ";
            }
            print(tmp);

            //���������
            std::vector<float>::iterator biggest = std::max_element(std::begin(score), std::end(score));
            //std::cout << "Max element is " << *biggest << " at position " << std::distance(std::begin(score), biggest) << std::endl;
            QString tmp3;
            tmp3 += QStringLiteral("�б���Ϊ��"); // | tmp3 += QStringLiteral("Clasification is :");
            tmp3 += QString::number(std::distance(std::begin(score), biggest));
            tmp3 += QStringLiteral("����߽����ٷֱȣ�"); // | tmp3 += QStringLiteral("Probability is :");
            tmp3 += QString::number(*biggest);
            print(tmp3);
            ui.output_label->setText(tmp3);
        }
    }
    else
        print("input error");
}
void HandWriting::about_msg()
{
    QMessageBox::information(NULL, "about", "Author: Felix. SCUT-BIPLAB All Copyright Reserved!", QMessageBox::Yes, QMessageBox::Yes);
}
