#include "Algo.h"

//#define DEBUG
void debug_imshow(char * name, Mat img)
{
#ifdef DEBUG
    imshow(name, img);
    waitKey(0);
#endif
}
/*
�㷨�๹�캯������ȡ����Ԥ�����ͼƬ��������������category_condition_probability | Algo class constructor will read all the training data and get the Conditional probability
*/
Algo::Algo()
{
    char img_path[80];
    //0. ��ȡͼ�� | read the image 
    for (int category = 0; category < CATEGORY; category++)   //10�� | 10 category
    {
        vector <double> probability;
        //1. ���������ȡ�������� | create and save the feature
        vector<vector<int>> summary;
        //2. ��ȡĳ����µ�$SAMPLE��ͼƬ������������ | get the $SAMPLE samples' feature in one of the category
        for (int sample = 0; sample < SAMPLE; sample++) 
        {
            sprintf(img_path, "./data/%d/%d-%d.bmp", category, category, sample+1);
            Mat src = cv::imread(img_path);
            vector <int> feature = get_feature(src);
            summary.push_back(feature);
            qDebug() << img_path;
        }
        //3. ����summary�󣬼�����������ÿ��λ��Ϊ1ʱ�����ڸ���ĸ��� | Traversing the summary and cal the clasification probability when location of this feature is 1
        for (int f_index = 0; f_index < FEATURE_DIM; f_index++)   //100ά�������� | 100 dim feature vector 
        {
            int sum = 0;
            for (int sample = 0; sample < SAMPLE; sample++)   //ÿ��$SAMPLE��ͼƬ | $SAMPLE images under every category
            {
                sum += summary[sample][f_index];
            }
            probability.push_back(double(sum+1)/float(SAMPLE + 2));
        }
        category_condition_probability.push_back(probability);
    }
}
vector<int> Algo::get_feature(Mat img)
{
    vector<int> feature;
    //0.0 ��ͼ��߽�������� | expansion the img border
    Scalar value = Scalar(255,255, 255);
    cv::copyMakeBorder(img, img, 400, 400, 400, 400, cv::BORDER_CONSTANT, value);
    //0. ת�Ҷ�ͼ | convert it to gray
    cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
    //1. ��ͼ���ֵ����ȷ���Ƕ�ֵͼ�� | convert to bin img
    cv::threshold(img, img, 128, 255, cv::THRESH_BINARY);
    debug_imshow("bin", img);

    int min_col = img.cols;
    int max_col = 0;
    int max_row = 0;
    int min_row = img.rows;
    //2. ��ͼ�������ӳ����ν�ȡ | Intercept the external rect
    for (int col = 0; col < img.cols; col++)
    {
        for (int row = 0; row < img.rows; row++)
        {
            if (img.at<uchar>(row, col) < 250)
            {
                if (col < min_col)
                {
                    min_col = col;
                }
                if (col > max_col)
                {
                    max_col = col;
                }
                if (row < min_row)
                {
                    min_row = row;
                }
                if (row > max_row)
                {
                    max_row = row;
                }
            }
        }
    }
    qDebug(" max_col is %d", max_col);
    //3.1 ��ȡ��С������������ | solve the center coor
    int middle_col = int((min_col + max_col) / 2);
    int middle_row = int((min_row + max_row) / 2);
    qDebug() << "max_col is" << max_col << "min_col is" << min_col << "max_row is" << max_row << "min_row is" << min_row << "img col is " << img.cols << "img row is "<< img.rows;
    if ((max_col - min_col == 0) || (abs(max_col - min_col) == img.cols) || (max_row - min_row == 0) || (abs(max_row - min_row) == img.rows))
    {
        return feature;
    }
    //3.1.0
    //3.2 �ж����з�������з���� | which direction is wider? col or row
    if ((max_col - min_col) >= (max_row - min_row))  //�п� | col width
    {
        //�����ĵ㣬������������ | expand the rows from the center point
        min_row = middle_row - int((max_col - min_col) / 2);
        max_row = middle_row + int((max_col - min_col) / 2);
    }
    else //�п� | row height
    {
        //�����ĵ㣬������������ | expand the cols from the center point
        min_col = middle_col - int((max_row - min_row) / 2);
        max_col = middle_col + int((max_row - min_row) / 2);
    }

    Rect rect(min_col, min_row, max_col - min_col, max_row - min_row);
    Mat image_roi = img(rect);
    debug_imshow("roi", image_roi);
    //4. �Խ�ȡ���ͼ��ֳ�10*10���������ÿ��С�����кڵ㣬��Ϊ1������Ϊ0 | divide the intercepted img to 10x10 region and set the location of feature to 1 if this exist the number pixel

    //4.1 resize�ɹ̶���С | resize to a constant size
    cv::resize(image_roi, image_roi, cv::Size(40, 40));
    debug_imshow("roi40", image_roi);
    
    Mat feature_img(Size(10,10), CV_8UC1);
    //4.2 ��������ͼ�񣬶�ÿ��С����Ľ����ж��Ƿ�������� | Traversing the whole img
    for (int col = 0; col < 10; col++)
    {
        for (int row = 0; row < 10; row++)
        {
            int count = 0;
            for (int col_sub = 0; col_sub < 4; col_sub ++)
            {
                for (int row_sub = 0; row_sub < 4; row_sub ++)
                {
                    if (image_roi.at<uchar>(row*4 + row_sub, col*4 + col_sub) < 200 )  //���С��������Ϊ1�� | if exist black pixel
                    {
                        count++;
                    }
                }
            }
            if (count > 1)
            {
                feature.push_back(1);
                feature_img.at<uchar>(row, col) = 0;
            }
            else
            {
                feature.push_back(0);
                feature_img.at<uchar>(row, col) = 255;
            }
        }
    }
    resize(feature_img, feature_img, Size(100, 100));
    debug_imshow("feature", feature_img);
    //5. ����100ά����������
    return feature;
}

/*
����Ϊ�������� | input:feature vector
����ֵΪ��� | return:clasification score
*/
vector<double> Algo::min_error_bayes(vector<int> feature)
{
    //category_condition_probability
    vector<double> probability;
    //0. ����100άfeature������ÿ��Ԫ�ض�Ӧ��ÿһ��ĸ��� | Traversing the feature, cal the probability under every categrory
    
    for (int sample = 0; sample < CATEGORY; sample++)
    {
        float plus = 1; //�˷���ʼ��Ϊ1 | init the plus to 1
        for (int f_index = 0; f_index < FEATURE_DIM; f_index++)   //100ά�������� | 100 dim feature vector
        {
            //1. ��ÿһ��ĸ�����ˣ��õ�����feature����ÿһ��ĸ��� | multiple the probability of every dim of feature and then get the the probability under every categrory
            if (feature[f_index] == 1)
            {
                plus = plus * category_condition_probability[sample][f_index];
            }
            else
            {
                plus = plus * (1-category_condition_probability[sample][f_index]);
            }
        }
        probability.push_back(plus);
    }
    //�����һ�� | normalize the output
    float sum = 0;
    for (int col = 0; col < CATEGORY; col++)
    {
        sum += probability[col];
    }
    for (int col = 0; col < CATEGORY; col++)
    {
        probability[col] = probability[col]/sum;
    }

    //2. ֱ�ӷ��������� | return clasification score
    return probability;
}