#include "shadingcorrection.h"
#include <iostream>
#include <string>
using namespace cv;
using namespace std;

ShadingCorrection::ShadingCorrection()
{
}

/**
 * @brief ShadingCorrection::correctShadingGaussian
 * Lightens dark parts of the rgbImg by spatial filtering in the V-channel
 * of HSV color space. Large values for sigma correct general tendencies of
 * illumination unevenness.
 * @param rgbImage the image to be corrected of uneven illumination
 * @param sigma standard deviation used for gaussian filtering. Low value -> small dark spots get corrected. High value -> general illumination tendency gets corrected
 * @return
 */
Mat ShadingCorrection::correctShadingGaussian(const Mat &rgbImage, const double &sigma){
    int sizes[] = {rgbImage.rows, rgbImage.cols};

    // convert to HSV color space
    Mat hsvImage;
    cvtColor(rgbImage,hsvImage,COLOR_BGR2HSV);

    // extract the V-chanel
    Mat vImage;
    extractChannel(hsvImage,vImage,2);

    // apply gaussian blur
    Mat blurredVImage;
    int ksize = int(0.01*rgbImage.rows)*2-1; // choose kernel size depending on image dimensions. ensure uneven
    GaussianBlur(vImage,blurredVImage,Size(ksize,ksize),sigma,sigma);

    // adjust vChanel
    Mat vAdjusted(2, sizes, CV_8UC1);
    vAdjusted = vImage - blurredVImage;
    Scalar meanSc = mean(vImage);
    int meanValue = int(sum(meanSc)[0]);
    Mat vMean = meanValue*Mat::ones(2, sizes, vAdjusted.type());
    vAdjusted += vMean;

    // merge the adjusted V-chanel into the hsvImage
    Mat hsvImageAdjusted(hsvImage);
    mixChannels(vAdjusted,hsvImageAdjusted,{0,2});

    // convert back to BGR color space
    Mat rgbImageAdjusted;
    cvtColor(hsvImageAdjusted,rgbImageAdjusted,COLOR_HSV2BGR);

    return rgbImageAdjusted;
}

class CavalcantiQuadricF:public MinProblemSolver::Function{
public:
    CavalcantiQuadricF(Mat V_in) : V(V_in) {}
    int getDims() const {
        return 6;
    }
    double calc(const double* P)const{
        double sum = 0;
        V.forEach<int>([&P, &sum](int &value, const int position[]) -> void{
            double x = double(position[0]);
            double y = double(position[1]);
            sum = sum + pow(double(value) - (P[0]*pow(x, 2) + P[1]*pow(y, 2) + P[2]*x*y + P[3]*x + P[4]*y + P[5]),2);
        });
        return sum;
    }
private:
    const Mat V;
};

/**
 * @brief correctShading corrects general shading tendency in image
 * @param rgbImage
 * @param k_prc percentage of image width (1-49) used for modeling of image shading
 * @param P_opt initial 6 coefficients for least square minimization
 * @param max_gray_for_model maximum grayscale value (0-255) for model. highger value will result in more aggressive adjustment
 * @return
 */
Mat ShadingCorrection::correctShadingCavalcanti(Mat rgbImage, int k_prc, InputOutputArray P_opt, int max_gray_for_model){

    // http://apps.jcns.fz-juelich.de/doku/sc/lmfit!!!!!!


    // convert to HSV color space
    Mat hsvImage;
    cvtColor(rgbImage,hsvImage,COLOR_BGR2HSV);

    // extract the V-chanel
    Mat vImage;
    extractChannel(hsvImage,vImage,2);
    //adjustedImage( "V", vImage);

    // extract ROI of the V-chanel which will be the base of our model
    int k = int(rgbImage.cols*k_prc/100);
    Mat vROI = unionOfCornerRegions(vImage, k);
    //adjustedImage( "V (ROI)", vROI);

    // solve the nonlinear least-squares problem to find optimal coefficients P_opt
    Ptr<DownhillSolver> solver=DownhillSolver::create();
    Ptr<MinProblemSolver::Function> ptr_F = makePtr<CavalcantiQuadricF>(vROI);
    Mat step = (Mat_<double>(6,1) << -0.5,-0.5,-0.5,-0.5,-0.5,-0.5);
    solver->setFunction(ptr_F);
    solver->setInitStep(step); // setTermCriteria für max. ausführungszeit?
    solver->minimize(P_opt);
    cout << P_opt.getMat() << endl;

    // generate the model Z using optimal coefficients P_opt in the function
    int sizes[] = {rgbImage.rows, rgbImage.cols};
    Mat Z = Mat::zeros(2, sizes, CV_64FC1); // single channel double with same size as image
    Z.forEach<double>([&P_opt](double &value, const int position[]) -> void{
        double x = double(position[0]);
        double y = double(position[1]);
        double z = P_opt.getMat().at<double>(0)*pow(x, 2)
                 + P_opt.getMat().at<double>(1)*pow(y, 2)
                 + P_opt.getMat().at<double>(2)*x*y
                 + P_opt.getMat().at<double>(3)*x
                 + P_opt.getMat().at<double>(4)*y
                 + P_opt.getMat().at<double>(5);
        value = z;
    });
    // normalize the values
    Mat Z_gray(2, sizes, CV_8UC1);
    normalize(Z,Z_gray,max_gray_for_model,0,NORM_MINMAX);
    Z_gray.convertTo(Z_gray, CV_8UC1);
    //adjustedImage("Z_gray", Z_gray);

    // adjust vChanel
    Mat vAdjusted(2, sizes, CV_16UC1);
    add(vImage,Z_gray,vAdjusted);
    vAdjusted.convertTo(vAdjusted, CV_8UC1);
    //adjustedImage("vAdjusted", vAdjusted);

    // merge the adjusted V-chanel into the hsvImage
    Mat hsvImageAdjusted(hsvImage);
    mixChannels(vAdjusted,hsvImageAdjusted,{0,2});

    // convert back to BGR color space
    Mat rgbImageAdjusted;
    cvtColor(hsvImageAdjusted,rgbImageAdjusted,COLOR_HSV2BGR);

    return rgbImageAdjusted;
}

/**
 * @brief unionOfCornerRegions extracts square-shaped ROI from each corner of the image
 * @param image
 * @param k length of square
 * @return the four ROIs combined to a single image with 4*pow(k,2) pixels
 */
Mat ShadingCorrection::unionOfCornerRegions(Mat image, int k){
    int height = image.rows;
    int width = image.cols;
    Mat imageTopleft(image,Rect(0,0,k,k));
    Mat imageBottomleft(image,Rect(0,height-k,k,k));
    Mat imageTopright(image,Rect(width-k,0,k,k));
    Mat imageBottomright(image,Rect(width-k,height-k,k,k));

    Mat image_roi_left;
    image_roi_left.push_back(imageTopleft);
    image_roi_left.push_back(imageBottomleft);
    Mat image_roi_right;
    image_roi_right.push_back(imageTopright);
    image_roi_right.push_back(imageBottomright);

    Mat image_roi;
    hconcat(image_roi_left,image_roi_right,image_roi);
    return image_roi;
}
