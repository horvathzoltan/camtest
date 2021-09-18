#include "imagehelper.h"

//ImageHelper::ImageHelper()
//{

//}

/**
 *  \brief Automatic brightness and contrast optimization with optional histogram clipping
 *  \param [in]src Input image GRAY or BGR or BGRA
 *  \param [out]dst Destination image
 *  \param clipHistPercent cut wings of histogram at given percent tipical=>1, 0=>Disabled
 *  \note In case of BGRA image, we won't touch the transparency
*/
void ImageHelper::BrightnessAndContrastAuto(const cv::Mat &src,
                                            cv::Mat &dst,
                                            float clipHistPercent)
{

    CV_Assert(clipHistPercent >= 0);
    CV_Assert((src.type() == CV_8UC1) || (src.type() == CV_8UC3) || (src.type() == CV_8UC4));

    int histSize = 256;
    float alpha, beta;
    double minGray = 0, maxGray = 0;

    //to calculate grayscale histogram
    cv::Mat gray;
    if (src.type() == CV_8UC1) gray = src;
    else if (src.type() == CV_8UC3) cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    else if (src.type() == CV_8UC4) cvtColor(src, gray, cv::COLOR_BGRA2GRAY);
    if (clipHistPercent == 0)
    {
        // keep full available range
        cv::minMaxLoc(gray, &minGray, &maxGray);
    }
    else
    {
        cv::Mat hist; //the grayscale histogram

        float range[] = { 0, 256 };
        const float* histRange = { range };
        bool uniform = true;
        bool accumulate = false;
        calcHist(&gray, 1, 0, cv::Mat (), hist, 1, &histSize, &histRange, uniform, accumulate);

        // calculate cumulative distribution from the histogram
        std::vector<float> accumulator(histSize);
        accumulator[0] = hist.at<float>(0);
        for (int i = 1; i < histSize; i++)
        {
            accumulator[i] = accumulator[i - 1] + hist.at<float>(i);
        }

        // locate points that cuts at required value
        float max = accumulator.back();
        clipHistPercent *= (max / 100.0); //make percent as absolute
        clipHistPercent /= 2.0; // left and right wings
        // locate left cut
        minGray = 0;
        while (accumulator[minGray] < clipHistPercent)
            minGray++;

        // locate right cut
        maxGray = histSize - 1;
        while (accumulator[maxGray] >= (max - clipHistPercent))
            maxGray--;
    }

    // current range
    float inputRange = maxGray - minGray;

    alpha = (histSize - 1) / inputRange;   // alpha expands current range to histsize range
    beta = -minGray * alpha;             // beta shifts current range so that minGray will go to 0

    // Apply brightness and contrast normalization
    // convertTo operates with saurate_cast
    src.convertTo(dst, -1, alpha, beta);

    // restore alpha channel from source
    if (dst.type() == CV_8UC4)
    {
        int from_to[] = { 3, 3};
        //CV_EXPORTS void mixChannels(const Mat* src, size_t nsrcs, Mat* dst, size_t ndsts,
                            //const int* fromTo, size_t npairs);
        cv::mixChannels(&src, 2, &dst,1, from_to, 1);
    }
    return;
}


auto ImageHelper::BrightnessAndContrast(const QImage &src,
                                        float clipHistPercent
                                        //,float*alpha, float*beta,
                                        //double *minG, double *maxG,
                                        //double *inputRange
                                        ) -> BCdata{

    cv::Mat m1 = QImage2MatCopy(src);
    return BrightnessAndContrast(m1, clipHistPercent);//, alpha, beta, minG, maxG,inputRange);
}

auto ImageHelper::BrightnessAndContrast(const cv::Mat &src, float clipHistPercent
                                        //,float*alpha, float*beta,
                                        //double *minGray, double *maxGray,
                                        //double *inputRange
                                        ) -> BCdata
{
    CV_Assert(clipHistPercent >= 0);
    CV_Assert((src.type() == CV_8UC1) || (src.type() == CV_8UC3) || (src.type() == CV_8UC4));

    int histSize = 256;
    //float alpha, beta;
    //double minGray = 0, maxGray = 0;
    BCdata d;

    d.minGray = 0;
    d.maxGray = 0;

    //to calculate grayscale histogram
    cv::Mat gray;
    if (src.type() == CV_8UC1) gray = src;
    else if (src.type() == CV_8UC3) cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    else if (src.type() == CV_8UC4) cvtColor(src, gray, cv::COLOR_BGRA2GRAY);
    if (clipHistPercent == 0)
    {
        // keep full available range
        cv::minMaxLoc(gray, &d.minGray, &d.maxGray);
    }
    else
    {
        cv::Mat hist; //the grayscale histogram

        float range[] = { 0, 256 };
        const float* histRange = { range };
        bool uniform = true;
        bool accumulate = false;
        calcHist(&gray, 1, 0, cv::Mat (), hist, 1, &histSize, &histRange, uniform, accumulate);

        // calculate cumulative distribution from the histogram
        std::vector<float> accumulator(histSize);
        accumulator[0] = hist.at<float>(0);
        for (int i = 1; i < histSize; i++)
        {
            accumulator[i] = accumulator[i - 1] + hist.at<float>(i);
        }

        // locate points that cuts at required value
        float max = accumulator.back();
        clipHistPercent *= (max / 100.0); //make percent as absolute
        clipHistPercent /= 2.0; // left and right wings
        // locate left cut
        d.minGray = 0;
        while (accumulator[d.minGray] < clipHistPercent)
            (d.minGray)++;

        // locate right cut
        d.maxGray = histSize - 1;
        while (accumulator[d.maxGray] >= (max - clipHistPercent))
            (d.maxGray)--;
    }

    // current range
    d.inputRange = d.maxGray - d.minGray;

    d.alpha = (histSize - 1) / (d.inputRange);   // alpha expands current range to histsize range
    d.beta = -(d.minGray) * (d.alpha);             // beta shifts current range so that minGray will go to 0

    return d;
}

QImage ImageHelper::BrightnessAndContrastAuto(const QImage& src, float clipHistPercent)
{
    cv::Mat m1 = QImage2MatCopy(src);
    cv::Mat m2;//(m1.cols, m1.rows, m1.type());
    BrightnessAndContrastAuto(m1,m2,clipHistPercent);

    QImage i = Mat2QImageRef(m2);
    return i;
}

///http://qtandopencv.blogspot.com/2013/08/how-to-convert-between-cvmat-and-qimage.html


auto ImageHelper::Mat2QImageRef(const cv::Mat & mat) -> QImage
{
    if(mat.data == nullptr) return QImage();
    auto format = CVFormat2QImageFormat(mat.type());
    QImage dest((const uchar *) mat.data,
                mat.cols, mat.rows, mat.step, format);//Format_BGR888
    return dest;
}

auto ImageHelper::Mat2QImageCopy(const cv::Mat & mat) -> QImage
{
    if(mat.data == nullptr) return QImage();
    auto format = CVFormat2QImageFormat(mat.type());
    QImage dest((const uchar *) mat.data,
                mat.cols, mat.rows, mat.step, format);
    return dest.copy();
}

auto ImageHelper::QImage2MatRef(const QImage & img) -> cv::Mat
{
    int format = QImageFormat2CVFormat(img.format());
    cv::Mat e(img.height(), img.width(),
              format, (void*)img.bits(), img.bytesPerLine());
    return e;
}

auto ImageHelper::QImage2MatCopy(const QImage & img) -> cv::Mat
{
    int format = QImageFormat2CVFormat(img.format());
    cv::Mat e(img.height(), img.width(),
              format, (void*)img.bits(), img.bytesPerLine());
    return e.clone();
}


/*
        Format_Invalid,
        Format_Mono,
        Format_MonoLSB,
        Format_Indexed8,
        Format_RGB32,
        Format_ARGB32,
        Format_ARGB32_Premultiplied,
        Format_RGB16,
        Format_ARGB8565_Premultiplied,
        Format_RGB666,
        Format_ARGB6666_Premultiplied,
        Format_RGB555,
        Format_ARGB8555_Premultiplied,
        Format_RGB888,
        Format_RGB444,
        Format_ARGB4444_Premultiplied,
        Format_RGBX8888,
        Format_RGBA8888,
        Format_RGBA8888_Premultiplied,
        Format_BGR30,
        Format_A2BGR30_Premultiplied,
        Format_RGB30,
        Format_A2RGB30_Premultiplied,
        Format_Alpha8,
        Format_Grayscale8,
        Format_RGBX64,
        Format_RGBA64,
        Format_RGBA64_Premultiplied,
        Format_Grayscale16,
        Format_BGR888,
*/

auto ImageHelper::QImageFormat2CVFormat(QImage::Format f) -> int
{
    switch(f){
    case QImage::Format_RGB32: return CV_8UC4;
    default: return -1;
    }
}

auto ImageHelper::CVFormat2QImageFormat(int f) -> QImage::Format
{
    switch(f){
    case CV_8UC4: return QImage::Format_RGB32;
    default: return QImage::Format::Format_Invalid;
    }
}
