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
    //Ϊ����ͷ����д�л�����ۺ���������л�����д����ʾ����Ϊ��д������л�����Ƶ������ʾ������Ƶ
    timer = new QTimer(this);
    //����ǰʱ�䳬ʱ������1000ms������m_pzEffectWidget����
    connect(timer, SIGNAL(timeout()), this, SLOT(get_frame()));
    timer->start(33.33);

    ////����رղۺ���
    //connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    //cam = new cv::VideoCapture(0);
    //cam->read(input_img);
    //cv::imshow("cam", input_img);
    cam = new cv::VideoCapture(0);
    if (!cam->isOpened())
    {
        QMessageBox::information(NULL, "Error", "No video device found!", QMessageBox::Yes, QMessageBox::Yes);
        exit(-1);
    }
    QApplication::setQuitOnLastWindowClosed(false);
    setAttribute(Qt::WA_DeleteOnClose);
}
HandWriting::~HandWriting()
{
    
    timer->stop();
    cv::destroyAllWindows();
    cam->release();
    exit(-1);
}
QImage Mat2QImage(cv::Mat const& src)
{
    cv::Mat temp; // make the same cv::Mat  
    cvtColor(src, temp, CV_BGR2RGB); // cvtColor Makes a copt, that what i need  
    QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888); //temp.setp()û��ʱ���ᵼ����ЩͼƬ��ת������б 
    dest.bits(); // enforce deep copy, see documentation  
                 // of QImage::QImage ( const uchar * data, int width, int height, Format format )  
    return dest;
}

int HandWriting::get_frame()
{
    //1.0 ��鰴ť״̬
    if (ui.Write_input->isChecked() && ! ui.Video_input->isChecked())
        //2.0 �������д���룬��������ͷ�Ƿ�򿪣�����ǣ���ر�
    {
            cv::destroyAllWindows();
    }
    if (!ui.Write_input->isChecked() && ui.Video_input->isChecked())
        //3.0 �������Ƶ���룬��������ͷ�Ƿ�رգ�����ǣ���򿪣�������ɼ�ͼ��input_img��
    {
        if ( !cam->isOpened())
        {
            QMessageBox::information(NULL, "Error", "No video device found!", QMessageBox::Yes, QMessageBox::Yes);
            exit(-1);
        }
        else
        {
            cam->read(input_img);
            cv::imshow("cam",input_img);
            //ui.input->
            //QPainter painter(ui.input);
            //painter.drawImage(640, 480, Mat2QImage(input_img));
            //QPainter painter;
            //painter.begin(ui.input);
            //painter.drawImage(QPoint(0, 0), Mat2QImage(input_img));
        }
    }
    //print("get_frame");
    return 0;
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
    Mat src;
    if (ui.Write_input->isChecked() && !ui.Video_input->isChecked())
        //2.0 �������д���룬��������ͷ�Ƿ�򿪣�����ǣ���ر�
    {
        QPixmap pix = QPixmap::grabWidget(ui.input);
        QImage img = pix.toImage();
        src = QImage2Mat(img);
    }
    if (!ui.Write_input->isChecked() && ui.Video_input->isChecked())
        //3.0 �������Ƶ���룬��������ͷ�Ƿ�رգ�����ǣ���򿪣�������ɼ�ͼ��input_img��
    {
        src = input_img;
    }
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
