#ifndef IMAGEENHANCERENUMS_H
#define IMAGEENHANCERENUMS_H

#include <QObject>
#include <QQmlEngine>

class ImageEnhancerEnums : public QObject {
    Q_OBJECT

    public:
        ImageEnhancerEnums() : QObject() {}

        enum ImageProcessingAlgorithm
        {
            Gaussian,
            ShadingCorrectionGaussian,
            ShadingCorrectionCavalcanti
        };
        Q_ENUMS(ImageProcessingAlgorithm)

        static void declareQML() {
            qmlRegisterType<ImageEnhancerEnums>("enhancer.enums", 1, 0, "ImageEnhancerEnums");
        }
};

#endif // IMAGEENHANCERENUMS_H
