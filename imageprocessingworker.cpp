#include "imageprocessingworker.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

ImageProcessingWorker::ImageProcessingWorker(const QUrl &imagePath, const ImageEnhancerEnums::ImageProcessingAlgorithm &algorithm)
{
    this->imagePath = imagePath;
    this->algorithm = algorithm;
}

ImageProcessingWorker::~ImageProcessingWorker() {
}

void ImageProcessingWorker::process() {
    qDebug() << "WORKER: processing started";
    bool loadedSuccess = loadImage(imagePath);
    if(loadedSuccess){
        Mat adjustedImage;
        switch(algorithm){
        case ImageEnhancerEnums::ImageProcessingAlgorithm::ShadingCorrectionCavalcanti: adjustedImage = applyShadingCorrectionCavalcanti(getImage());break;
        case ImageEnhancerEnums::ImageProcessingAlgorithm::ShadingCorrectionGaussian: adjustedImage = applyShadingCorrectionGauss(getImage());break;
        case ImageEnhancerEnums::ImageProcessingAlgorithm::Gaussian: adjustedImage = applyGaussian(getImage()); break;
        }
        saveImage(adjustedImage);
    } else {
        error(QString("could not load the image"+imagePath.path()));
    }
    qDebug() << "WORKER: processing finished";
    emit finished();
}

bool ImageProcessingWorker::loadImage(const QUrl &value){
    String path = value.path().toStdString();
    if (path.length() > 0){
        image = imread(path, IMREAD_COLOR);
        ImageProcessingWorker::setImage(image);
        return true;
    }else{
        return false;
    }
}

Mat ImageProcessingWorker::getImage() const
{
    return image;
}

void ImageProcessingWorker::setImage(const Mat &value)
{
    image = value;
}

Mat ImageProcessingWorker::getEnhancedImage() const
{
    return enhancedImage;
}

void ImageProcessingWorker::setEnhancedImage(const Mat &value)
{
    enhancedImage = value;
}

void ImageProcessingWorker::saveImage(const Mat &value)
{
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    QString workingDir = QDir::currentPath();
    String fileName = workingDir.toStdString()+"/enhanced.jpg";
    imwrite(fileName,value,compression_params);
    QUrl fileUrl = QUrl(QString::fromStdString("file://"+fileName));
    qDebug() << "WORKER: saving image to: " << fileUrl;
    emit fileSaved(fileUrl);
}

Mat ImageProcessingWorker::applyShadingCorrectionCavalcanti(const Mat &value){
    qDebug() << "WORKER: applying Shading Correction (Cavalcanti)";
    Mat adjustedImage;
    Mat P_init = Mat::ones(6, 1, CV_64FC1);
    int k_prc = 25;
    int max_gray_for_model = 150;
    adjustedImage = ShadingCorrection::correctShadingCavalcanti(value, k_prc, P_init, max_gray_for_model);
    return adjustedImage;
}

Mat ImageProcessingWorker::applyShadingCorrectionGauss(const Mat &value){
    qDebug() << "WORKER: applying Shading Correction (Gauss)";
    Mat adjustedImage;
    double sigma = 10;
    adjustedImage = ShadingCorrection::correctShadingGaussian(value, sigma);
    return adjustedImage;
}

Mat ImageProcessingWorker::applyGaussian(const Mat &value){
    qDebug() << "WORKER: applying Gaussian";
    Mat adjustedImage;
    int ksize = 51;
    double sigma = 5;
    GaussianBlur(value,adjustedImage,Size(ksize,ksize),sigma,sigma);
    return adjustedImage;
}
