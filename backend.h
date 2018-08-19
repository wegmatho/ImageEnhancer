#ifndef BACKEND_H
#define BACKEND_H

#include <opencv2/core.hpp>
#include <QObject>
#include <QUrl>
#include <QDir>

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl originalImagePath READ originalImagePath WRITE setOriginalImagePath NOTIFY originalImagePathChanged)
    Q_PROPERTY(QUrl enhancedImagePath READ enhancedImagePath WRITE setEnhancedImagePath NOTIFY enhancedImagePathChanged)
    Q_PROPERTY(ImageProcessingAlgorithm algorithm READ algorithm WRITE setAlgorithm NOTIFY algorithmChanged )

public:
    explicit BackEnd(QObject *parent = nullptr);

    enum ImageProcessingAlgorithm { Gaussian, ShadingCorrectionGaussian, ShadingCorrectionCavalcanti };
    Q_ENUM(ImageProcessingAlgorithm)

    QUrl originalImagePath();
    void setOriginalImagePath(const QUrl &originalImagePath);

    QUrl enhancedImagePath();
    void setEnhancedImagePath(const QUrl &enhancedImagePath);

    ImageProcessingAlgorithm algorithm();
    void setAlgorithm(const ImageProcessingAlgorithm &algorithm);

    Q_INVOKABLE void applyAlgorithm();

signals:
    void originalImagePathChanged();
    void enhancedImagePathChanged();
    void algorithmChanged(ImageProcessingAlgorithm);

private:
    QUrl m_originalImagePath, m_enhancedImagePath;
    ImageProcessingAlgorithm m_algorithm;
    cv::Mat image, enhancedImage;
    cv::Mat getImage() const;
    void setImage(const cv::Mat &value);
    cv::Mat getEnhancedImage() const;
    void setEnhancedImage(const cv::Mat &value);
    bool loadImage(const QUrl &value);
    QUrl saveImage(const cv::Mat &value);
    cv::Mat applyShadingCorrectionGauss(const cv::Mat &value);
    cv::Mat applyShadingCorrectionCavalcanti(const cv::Mat &value);
    cv::Mat applyGaussian(const cv::Mat &value);
};

#endif // BACKEND_H
