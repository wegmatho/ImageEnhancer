#ifndef BACKEND_H
#define BACKEND_H

#include <opencv2/core.hpp>
#include "imageprocessingworker.h"
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QUrl>
#include <QDir>

class BackEnd : public QObject {
    Q_OBJECT
    Q_PROPERTY(QUrl originalImagePath READ originalImagePath WRITE setOriginalImagePath NOTIFY originalImagePathChanged)
    Q_PROPERTY(QUrl enhancedImagePath READ enhancedImagePath WRITE setEnhancedImagePath NOTIFY enhancedImagePathChanged)
    Q_PROPERTY(ImageEnhancerEnums::ImageProcessingAlgorithm algorithm READ algorithm WRITE setAlgorithm NOTIFY algorithmChanged)
    Q_PROPERTY(bool isProcessing READ isProcessing WRITE setIsProcessing NOTIFY isProcessingChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);

    QUrl originalImagePath();
    void setOriginalImagePath(const QUrl &originalImagePath);

    QUrl enhancedImagePath();
    void setEnhancedImagePath(const QUrl &enhancedImagePath);

    ImageEnhancerEnums::ImageProcessingAlgorithm algorithm();
    void setAlgorithm(const ImageEnhancerEnums::ImageProcessingAlgorithm &algorithm);

    bool isProcessing();
    void setIsProcessing(const bool &isProcessing);

    Q_INVOKABLE void startAlgorithm();

    static void declareQML() {
        qmlRegisterType<BackEnd>("enhancer.backend", 1, 0, "BackEnd");
    }

public slots:
    void workerFinished(const QUrl &fileName);
    void workerError(QString err);

signals:
    void originalImagePathChanged();
    void enhancedImagePathChanged();
    void algorithmChanged(ImageEnhancerEnums::ImageProcessingAlgorithm);
    void isProcessingChanged();

private:
    QUrl m_originalImagePath, m_enhancedImagePath;
    ImageEnhancerEnums::ImageProcessingAlgorithm m_algorithm;
    bool m_isProcessing = false;
};

#endif // BACKEND_H
