#ifndef     __MY_CAMERA__
#define     __MY_CAMERA__

#include <QDebug>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <QPixmap>
#include <QLabel>
#include <QThread>
#include <QImage>
#include <QMutex>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
#include <libavutil/dict.h> 
}

class MyCamera : public QThread{
Q_OBJECT
public:
    MyCamera(QObject *parent = nullptr);
    ~MyCamera();
    void openCamera(QString cameraName);
    void InitCamera();
    void run() override;
    void stopCamera();
    cv::Mat getMat();

    signals:
    void sigGetFrame(cv::Mat fr);
protected:

private:
    AVPacket *pPacket = NULL;  
    AVFrame *pFrame = NULL; 
    AVFormatContext *pFormatCtx = NULL; 
    AVCodecContext *codecCtx = NULL;
    bool isRunning = false;
    int videoStreamIndex = -1;

    // opencv 的图像帧
    cv::Mat mat;

    QMutex mutex_;
};


#endif