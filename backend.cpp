#include "backend.h"
#include "shadingcorrection.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

BackEnd::BackEnd(QObject *parent) : QObject(parent)
{
}

QUrl BackEnd::originalImagePath()
{
    return m_originalImagePath;
}

void BackEnd::setOriginalImagePath(const QUrl &originalImagePath)
{
    if (originalImagePath == m_originalImagePath)
        return;

    m_originalImagePath = originalImagePath;
    emit originalImagePathChanged();
    applyAlgorithm();
}

QUrl BackEnd::enhancedImagePath()
{
    return m_enhancedImagePath;
}

void BackEnd::setEnhancedImagePath(const QUrl &enhancedImagePath)
{
    if (enhancedImagePath == m_enhancedImagePath)
        return;

    m_enhancedImagePath = enhancedImagePath;
}

BackEnd::ImageProcessingAlgorithm BackEnd::algorithm(){
    return m_algorithm;
}
void BackEnd::setAlgorithm(const ImageProcessingAlgorithm &algorithm){
    if (algorithm == m_algorithm)
        return;

    m_algorithm = algorithm;
    emit algorithmChanged(m_algorithm);
    applyAlgorithm();
}

void BackEnd::applyAlgorithm(){
    bool loadedSuccess = loadImage(m_originalImagePath);
    if(loadedSuccess){
        Mat adjustedImage;
        switch(BackEnd::algorithm()){
        case ImageProcessingAlgorithm::ShadingCorrectionCavalcanti: adjustedImage = applyShadingCorrectionCavalcanti(BackEnd::getImage());break;
        case ImageProcessingAlgorithm::ShadingCorrectionGaussian: adjustedImage = applyShadingCorrectionGauss(BackEnd::getImage());break;
        case ImageProcessingAlgorithm::Gaussian: adjustedImage = applyGaussian(BackEnd::getImage()); break;
        }

        QUrl filename = saveImage(adjustedImage);
        BackEnd::setEnhancedImagePath(filename);
        emit enhancedImagePathChanged();
    }
}

bool BackEnd::loadImage(const QUrl &value){
    String path = value.path().toStdString();
    if (path.length() > 0){
        image = imread(path, IMREAD_COLOR);
        BackEnd::setImage(image);
        return true;
    }else{
        return false;
    }
}

Mat BackEnd::getImage() const
{
    return image;
}

void BackEnd::setImage(const Mat &value)
{
    image = value;
}

Mat BackEnd::getEnhancedImage() const
{
    return enhancedImage;
}

void BackEnd::setEnhancedImage(const Mat &value)
{
    enhancedImage = value;
}

QUrl BackEnd::saveImage(const Mat &value)
{
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    QString workingDir = QDir::currentPath();
    String fileName = workingDir.toStdString()+"/enhanced.jpg";
    imwrite(fileName,value,compression_params);
    return QUrl(QString::fromStdString("file://"+fileName));
}

Mat BackEnd::applyShadingCorrectionCavalcanti(const Mat &value){
    Mat adjustedImage;
    Mat P_init = Mat::ones(6, 1, CV_64FC1);
    int k_prc = 25;
    int max_gray_for_model = 150;
    adjustedImage = ShadingCorrection::correctShadingCavalcanti(value, k_prc, P_init, max_gray_for_model);
    return adjustedImage;
}

Mat BackEnd::applyShadingCorrectionGauss(const Mat &value){
    Mat adjustedImage;
    double sigma = 10;
    adjustedImage = ShadingCorrection::correctShadingGaussian(value, sigma);
    return adjustedImage;
}

Mat BackEnd::applyGaussian(const Mat &value){
    Mat adjustedImage;
    int ksize = 51;
    double sigma = 5;
    GaussianBlur(value,adjustedImage,Size(ksize,ksize),sigma,sigma);
    return adjustedImage;
}
