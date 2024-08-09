#ifndef     __MY_CAMERA__
#define     __MY_CAMERA__

#include <QDebug>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <QPixmap>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
#include <libavutil/dict.h> 
}

class MyCamera{
public:
    void openCamera();
private:



};


#endif