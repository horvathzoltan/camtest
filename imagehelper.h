#ifndef IMAGEHELPER_H
#define IMAGEHELPER_H

#include <opencv4/opencv2/opencv.hpp>

#include <QImage>

class ImageHelper
{
public:
    //ImageHelper();
    struct BCdata{
        float alpha, beta;
        double minGray, maxGray;
        double inputRange;
        int brightness, contrast, iso;
        int ix;
    };

    static void BrightnessAndContrastAuto(const cv::Mat &src,
                                          cv::Mat &dst,
                                          float clipHistPercent = 0);

    static QImage BrightnessAndContrastAuto(const QImage &src,
                                          float clipHistPercent = 0);

    static BCdata BrightnessAndContrast(const cv::Mat &src,float clipHistPercent);

    static BCdata BrightnessAndContrast(const QImage &src, float clipHistPercent);//,
                                      //float *alpha, float *beta,
                                      //double *minG, double *maxG,
                                      //double *inputRange);

    static QImage Mat2QImageRef(const cv::Mat & src);
    static QImage Mat2QImageCopy(const cv::Mat & src);
    static cv::Mat QImage2MatRef(const QImage &src);
    static cv::Mat QImage2MatCopy(const QImage &src);
    static int QImageFormat2CVFormat(QImage::Format f);
    static QImage::Format CVFormat2QImageFormat(int f);

};

#endif // IMAGEHELPER_H
