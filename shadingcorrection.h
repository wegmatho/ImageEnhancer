#ifndef SHADINGCORRECTION_H
#define SHADINGCORRECTION_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

class ShadingCorrection {

public:
    ShadingCorrection();
    static cv::Mat correctShadingGaussian(const cv::Mat &rgbImage, const double &sigma);
    static cv::Mat correctShadingCavalcanti(cv::Mat rgbImage, int k_prc, cv::InputOutputArray P_opt, int max_gray_for_model);
    static cv::Mat unionOfCornerRegions(cv::Mat image, int k);

};

#endif // SHADINGCORRECTION_H
