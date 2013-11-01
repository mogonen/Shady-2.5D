#include "shadingcolorextractor.h"
#include <iostream>
#include <sstream>

using namespace cv;

//ShadingColorExtractor::ShadingColorExtractor(const cv::Mat& org_img, const float *landMarks, int num_lm)
//{
//    m_orgImage = org_img.clone();
//}

ShadingColorExtractor::ShadingColorExtractor(const SymmetryQuad &sq)
{
    m_orgImage = sq.m_rawImage.clone();
    m_vec2Vertices = sq.GetVerticesVector();
}

cv::Mat ShadingColorExtractor::SkinColorSegment()
{
    SampleLandMark();
//    cvtColor(frame, frame, CV_BGR2HSV);
//    GaussianBlur(frame, frame, Size(7,7), 1, 1);
//    for(int r=0; r<frame.rows; ++r){
//        for(int c=0; c<frame.cols; ++c)
//            // 0<H<0.25  -   0.15<S<0.9    -    0.2<V<0.95
//            if( (frame(r,c)[0]>5) && (frame(r,c)[0] < 17) && (frame(r,c)[1]>38) && (frame(r,c)[1]<250) && (frame(r,c)[2]>51) && (frame(r,c)[2]<242) ); // do nothing
//            else for(int i=0; i<3; ++i)	frame(r,c)[i] = 0;
//    }

//    /* BGR CONVERSION AND THRESHOLD */
//    Mat1b frame_gray;
//    cvtColor(frame, frame, CV_HSV2BGR);
//    cvtColor(frame, frame_gray, CV_BGR2GRAY);
//    threshold(frame_gray, frame_gray, 60, 255, CV_THRESH_BINARY);
//    morphologyEx(frame_gray, frame_gray, CV_MOP_ERODE, Mat1b(3,3,1), cv::Point2i(-1, -1), 3);
//    morphologyEx(frame_gray, frame_gray, CV_MOP_OPEN, Mat1b(7,7,1), cv::Point2i(-1, -1), 1);
//    morphologyEx(frame_gray, frame_gray, CV_MOP_CLOSE, Mat1b(9,9,1), cv::Point2i(-1, -1), 1);

//    medianBlur(frame_gray, frame_gray, 15);

    return Mat();
}


void ShadingColorExtractor::SampleLandMark()
{
    //All the polygones: LeftBrow, RightBrow,LeftEye, RightEye, Nose,Lips, Whole face
    vector< vector<cv::Point2i> > polys(7);

    //Form Left Eye Brow Polygon
    polys[0].insert(polys[0].end(),m_vec2Vertices.begin()+16,m_vec2Vertices.begin()+22);
    //FormRight Eye Brow Polygon
    polys[1].insert(polys[1].end(),m_vec2Vertices.begin()+22,m_vec2Vertices.begin()+28);
    //Form Left Eye Polygon
    polys[2].insert(polys[2].end(),m_vec2Vertices.begin()+30,m_vec2Vertices.begin()+38);
    //Form Right Eye Polygon
    polys[3].insert(polys[3].end(),m_vec2Vertices.begin()+40,m_vec2Vertices.begin()+48);
    //Form Nose Polygon
    polys[4].insert(polys[4].end(),m_vec2Vertices.begin()+54,m_vec2Vertices.begin()+59);
    //Form Lips Polygon
    polys[5].insert(polys[5].end(),m_vec2Vertices.begin()+59,m_vec2Vertices.begin()+66);
    polys[5].insert(polys[5].end(),m_vec2Vertices.begin()+72,m_vec2Vertices.begin()+77);

    //Form Whole Face
    polys[6].insert(polys[6].end(),m_vec2Vertices.begin()+0,m_vec2Vertices.begin()+13);
    polys[6].insert(polys[6].end(),m_vec2Vertices.begin()+77,m_vec2Vertices.begin()+79);
    polys[6].insert(polys[6].end(),m_vec2Vertices.begin()+13,m_vec2Vertices.begin()+16);
    polys[6].insert(polys[6].end(),m_vec2Vertices.begin()+79,m_vec2Vertices.begin()+81);

    cv::Mat temp = Mat::zeros(m_orgImage.rows,m_orgImage.cols,CV_8UC1);

    fillPoly(temp,polys,Scalar(255));
    cv::erode(temp,temp,cv::Mat(),cv::Point2i(-1,-1),5);



    Mat crop_img(m_orgImage.rows,m_orgImage.cols,CV_8UC3);
    m_orgImage.copyTo(crop_img, temp);

    Mat HSV_img;
    cvtColor(crop_img, HSV_img, CV_BGR2HSV);
    vector<Mat> ill_img;
    split(HSV_img, ill_img);

    cvtColor(crop_img, HSV_img, CV_BGR2HSV);


    int key;
    int V_color=50;

    cv::Point2i light = CalLightingPos(ill_img[2], temp);

    Mat t_temp = Mat::zeros(m_orgImage.rows,m_orgImage.cols,CV_8UC3);

    double minValue, maxValue;
//    namedWindow("123",CV_NORMAL);
//    imshow("123",ill_img[2]);
    ColorAnalysis(ill_img[2],temp, minValue, maxValue);

    std::cout<<"minV: "<<(int)(100*minValue/255)<<"maxV: "<<(int)(100*maxValue/255)<<std::endl;
    BuildHSVImage(ill_img,minValue, 2).copyTo(t_temp, temp);
    circle(t_temp,light,3,Scalar(0,255,255),-1);
    flip(cvtTo4Channle(t_temp, temp), m_darkImage,0);


    BuildHSVImage(ill_img,maxValue, 2).copyTo(t_temp, temp);
    circle(t_temp,light,3,Scalar(0,255,255),-1);
    flip(cvtTo4Channle(t_temp, temp), m_brightImage,0);

//    imwrite("bright.png",m_brightImage);

//    imwrite("cropped.png",crop_img);

//    circle(t_temp,light,3,Scalar(0,255,255),-1);
//    imshow("123",t_temp);


//    namedWindow("123",CV_NORMAL);
//    circle(t_temp,light,3,Scalar(0,255,255),-1);
//    imshow("123",temp);

//    Mat masked_img;
//    ill_img[2].copyTo(masked_img, temp);

//    ColorAnalysis(ill_img);
//    Mat t_temp;
//    string text = "Val";
//    int channel = 2;
//    float V_color_f = 1;
//    while(key = waitKey(0))
//    {
//        t_temp = Mat(m_orgImage.rows,m_orgImage.cols,CV_8UC3);
//        switch(key)
//        {
//        case 27:
//            return;
//        case '=':
//            V_color += 10;
//            if(V_color>255)
//                V_color = 255;
//            BuildHSVImage(ill_img,V_color, channel).copyTo(t_temp, temp);
//            break;
//        case '-':
//            V_color -= 10;
//            if(V_color<0)
//                V_color = 0;
//            BuildHSVImage(ill_img,V_color, channel).copyTo(t_temp, temp);
//            break;

//        case '+':
//            V_color_f *= 1.1;
//            BuildHSVImage(ill_img,V_color_f, channel,false).copyTo(t_temp, temp);
//            break;
//        case '_':
//            V_color_f *= 0.9;
//            BuildHSVImage(ill_img,V_color_f, channel,false).copyTo(t_temp, temp);
//            break;

//        case '1':
//            t_temp = ill_img[0];
//            channel = 0;
//            text = "Hue";
//            break;
//        case '2':
//            t_temp = ill_img[1];
//            channel = 1;
//            text = "Sat";
//            break;
//        case '3':
//            t_temp = ill_img[2];
//            channel = 2;
//            text = "Val";
//            break;
//        case '4':
//            t_temp = crop_img;
//            break;

//        }
//        stringstream st;
//        st<<V_color;
//        putText(t_temp, text+" "+st.str(), cv::Point2i(20,20),
//            FONT_HERSHEY_COMPLEX_SMALL,1, cvScalar(255,255,255), 1, CV_AA);
//        imshow("123",t_temp);
//    }
}

cv::Mat ShadingColorExtractor::BuildHSVImage(vector<cv::Mat> in_img, float HSV_color, int channel, bool mode)
{
    Mat temp;

    if(mode)
        in_img[channel] =  Mat (in_img[channel].rows,in_img[channel].cols,in_img[channel].type(),HSV_color);
    else
    {
        Mat tt = in_img[channel].clone();
        tt *= HSV_color;
        in_img[channel] =  tt;//Mat (in_img[channel].rows,in_img[channel].cols,in_img[channel].type(),HSV_color);

    }
    merge(in_img, temp);
    cvtColor(temp,temp,CV_HSV2BGR);
    return temp;
}


void ShadingColorExtractor::ColorAnalysis(cv::Mat &in_img, cv::Mat &mask, double &minValue, double &maxValue, double per)
{
    minValue = -1;
    maxValue = -1;
    float range[] = { 0, 255 } ;
    const float* histRange = { range };
    int histSize = 256;
    Mat hist;
    calcHist(&in_img, 1, 0, mask, hist,1,&histSize,&histRange,true, false);
    unsigned long int sum = 0;
    int total_p = countNonZero(mask);
    for(int i=0;i<255;i++)
    {
        sum += hist.at<float>(i);
        if(sum>total_p*per&&minValue == -1)
        {
            minValue = i;
        }
        if(sum>total_p*(1-per))
        {
            maxValue = i;
            break;
        }
    }
}

cv::Point2i ShadingColorExtractor::CalLightingPos(Mat in_value, Mat mask)
{

    Mat non_zero;
    findNonZero(mask,non_zero);
    std::cout<<"type"<<non_zero.type()<<"sss";
    Mat light_pos(non_zero.rows,3,CV_32FC1);

    //min max of three dimension
    float x_min=mask.cols,x_max=-1,y_min = mask.rows,y_max=-1,z_min=255,z_max = -1;
    for(int i=0;i<non_zero.rows;i++)
    {
        cv::Point2i pos = non_zero.at<cv::Point2i>(i);
        light_pos.at<float>(i,0) = pos.x;
        if(pos.x>x_max)
            x_max = pos.x;
        if(pos.x<x_min)
            x_min = pos.x;
        light_pos.at<float>(i,1) = pos.y;
        if(pos.y>y_max)
            y_max = pos.y;
        if(pos.y<y_min)
            y_min = pos.y;
        light_pos.at<float>(i,2) = (float)in_value.at<unsigned char>(pos);
        if(light_pos.at<float>(i,2)>z_max)
            z_max = light_pos.at<float>(i,2);
        if(light_pos.at<float>(i,2)<z_min)
            z_min = light_pos.at<float>(i,2);
    }


    for(int i=0;i<non_zero.rows;i++)
    {
        light_pos.at<float>(i,0) = (light_pos.at<float>(i,0)-x_min)/(x_max-x_min);
        light_pos.at<float>(i,1) = (light_pos.at<float>(i,1)-y_min)/(y_max-y_min);
        light_pos.at<float>(i,2) = (light_pos.at<float>(i,2)-z_min)/(z_max-z_min);
    }

    double minValue, maxValue;
    minMaxLoc(light_pos.col(2),&minValue,&maxValue);
    std::cout<<"min "<<minValue<<" max "<<maxValue;

    PCA PCAProjector;
    PCAProjector(light_pos, Mat(), CV_PCA_DATA_AS_ROW, 3);
    Mat xyz_mean = PCAProjector.mean;
    Mat eigen_V = PCAProjector.eigenvectors;

    Mat eigen_Val = PCAProjector.eigenvalues;
    std::cout<<eigen_V;
    //convert back to image space
    if(eigen_V.at<float>(0,2)>0)
    {
        float pos_x = (eigen_V.at<float>(0,0)+xyz_mean.at<float>(0))*(x_max-x_min)+x_min;
        float pos_y = (eigen_V.at<float>(0,1)+xyz_mean.at<float>(1))*(y_max-y_min)+y_min;
        return cv::Point2i(pos_x,pos_y);
    }
    else
    {
        float pos_x = (-eigen_V.at<float>(0,0)+xyz_mean.at<float>(0))*(x_max-x_min)+x_min;
        float pos_y = (-eigen_V.at<float>(0,1)+xyz_mean.at<float>(1))*(y_max-y_min)+y_min;
        return cv::Point2i(pos_x,pos_y);
    }
}

cv::Mat ShadingColorExtractor::cvtTo4Channle(cv::Mat source, cv::Mat Mask)
{
    cv::Mat newSrc = cv::Mat(source.rows,source.cols,CV_8UC4);
    Mat in[] = { source, Mask };
    int from_to[] = { 2,0, 1,1, 0,2, 3,3 };
    mixChannels( in, 2, &newSrc, 1, from_to, 4 );
    return newSrc;
}
