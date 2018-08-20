#include "backend.h"
#include <iostream>
using namespace std;

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
    startAlgorithm();
}

QUrl BackEnd::enhancedImagePath()
{
    return m_enhancedImagePath;
}

void BackEnd::setEnhancedImagePath(const QUrl &enhancedImagePath)
{
    if (enhancedImagePath == m_enhancedImagePath){
        // always trigger the event so QML reloads the image
        emit enhancedImagePathChanged();
        return;
    }

    m_enhancedImagePath = enhancedImagePath;
    emit enhancedImagePathChanged();
}

ImageEnhancerEnums::ImageProcessingAlgorithm BackEnd::algorithm(){
    return m_algorithm;
}
void BackEnd::setAlgorithm(const ImageEnhancerEnums::ImageProcessingAlgorithm &algorithm){
    if (algorithm == m_algorithm)
        return;

    m_algorithm = algorithm;
    emit algorithmChanged(m_algorithm);
    startAlgorithm();
}

bool BackEnd::isProcessing()
{
    return m_isProcessing;
}

void BackEnd::setIsProcessing(const bool &isProcessing)
{
    if (isProcessing == m_isProcessing)
        return;

    m_isProcessing = isProcessing;
    emit isProcessingChanged();
}

void BackEnd::startAlgorithm(){
    if(isProcessing()){
        qDebug() << "BACKEND: busy (tried to start new worker thread)";
    } else{
        setIsProcessing(true);
        qDebug() << "BACKEND: starting worker thread";
        QThread* thread = new QThread;
        ImageProcessingWorker* worker = new ImageProcessingWorker(originalImagePath(), algorithm());
        worker->moveToThread(thread);
        connect(worker, SIGNAL (error(QString)), this, SLOT (workerError(QString)));
        connect(thread, SIGNAL (started()), worker, SLOT (process()));
        connect(worker, SIGNAL (fileSaved(QUrl)), this, SLOT (workerFinished(QUrl)));
        connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
        connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
        connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
        thread->start();
        qDebug() << "BACKEND: worker thread started";
    }
}

void BackEnd::workerFinished(const QUrl &fileName) {
    qDebug() << "BACKEND: got worker finished event with file: " << fileName;
    BackEnd::setEnhancedImagePath(fileName);
    setIsProcessing(false);
}

void BackEnd::workerError(QString err)
{
    qDebug() << "BACKEND: Error received from worker: " << err;
    setIsProcessing(false);
}
