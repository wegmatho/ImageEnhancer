#ifndef IMAGEPROCESSINGWORKER_H
#define IMAGEPROCESSINGWORKER_H

#include "imageenhancerenums.h"
#include "shadingcorrection.h"
#include <QObject>
#include <QDebug>
#include <QUrl>
#include <QDir>

class ImageProcessingWorker : public QObject {
    Q_OBJECT

public:
    ImageProcessingWorker(const QUrl &imagePath, const ImageEnhancerEnums::ImageProcessingAlgorithm &algorithm);
    ~ImageProcessingWorker();

public slots:
    void process();

signals:
    void finished();
    void fileSaved(const QUrl &outFileName);
    void error(QString err);

private:
    QUrl imagePath;
    ImageEnhancerEnums::ImageProcessingAlgorithm algorithm;
    cv::Mat image, enhancedImage;
    cv::Mat getEnhancedImage() const;
    void setEnhancedImage(const cv::Mat &value);
    bool loadImage(const QUrl &value);
    cv::Mat getImage() const;
    void setImage(const cv::Mat &value);
    void saveImage(const cv::Mat &value);
    cv::Mat applyShadingCorrectionGauss(const cv::Mat &value);
    cv::Mat applyShadingCorrectionCavalcanti(const cv::Mat &value);
    cv::Mat applyGaussian(const cv::Mat &value);
};

#endif // IMAGEPROCESSINGWORKER_H
