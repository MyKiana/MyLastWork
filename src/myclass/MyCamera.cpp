#include "MyCamera.h"


 MyCamera::MyCamera(QObject *parent) : QThread(parent) {
    // 初始化设备
    
    InitCamera();
}

MyCamera::~MyCamera() {
    // 释放资源
    avformat_close_input(&pFormatCtx);
    avcodec_send_packet(codecCtx, NULL); // 向解码器发送空包，以便清空解码器
    avcodec_receive_frame(codecCtx, pFrame); // 接收解码器中的所有帧 
    av_packet_free(&pPacket);
    av_frame_free(&pFrame); 
    avcodec_free_context(&codecCtx);
}


void MyCamera::InitCamera(){
    // 注册所有文件格式和编解码器  
    avformat_network_init();

    avdevice_register_all();

    // 分配空间
    pPacket = av_packet_alloc();  
    pFrame = av_frame_alloc();  

}

void MyCamera::openCamera(QString cameraName){
    // 打开摄像头
    if (avformat_open_input(&pFormatCtx, cameraName.toStdString().c_str(), NULL, NULL) != 0) {  
        // 错误处理  
        qDebug() << "Could not open video device.";
        return ;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {  
        qDebug() << "Could not find stream information.";  
        avformat_close_input(&pFormatCtx);  
        return ;  
    }
    videoStreamIndex = -1;  
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {  
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {  
            videoStreamIndex = i;  
            break;  
        }  
    }  
    if (videoStreamIndex == -1) {  
        qDebug() << "Could not find a video stream.";  
        avformat_close_input(&pFormatCtx);  
        return;  
    } 
    // 找到视频流的解码器
    AVCodecParameters *codecpar = pFormatCtx->streams[videoStreamIndex]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        qDebug() << "Failed to find codec for video stream.";
        return;
    }
    codecCtx = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(codecCtx, codecpar) < 0) {
        qDebug() << "Failed to copy codec parameters.";
        return;
    }
    if (avcodec_open2(codecCtx, codec, NULL) < 0) { 
        qDebug() << "Failed to open codec.";
        return;
    }  
    isRunning = true;
    this->start();
}

void MyCamera::run(){
    // 分配一个转换上下文
    struct SwsContext *sws_ctx = sws_getContext(
        codecCtx->width,
        codecCtx->height,
        codecCtx->pix_fmt,
        codecCtx->width,
        codecCtx->height,
        AV_PIX_FMT_BGR24,
        SWS_BILINEAR,
        NULL, NULL, NULL
    );
    while (isRunning)
    {
        if(av_read_frame(pFormatCtx, pPacket) >= 0) {  
            if (pPacket->stream_index == videoStreamIndex) {  

                // 将AVPacket数据送入解码器
                int ret = avcodec_send_packet(codecCtx, pPacket);
                if (ret < 0) {
                    qDebug() << "Error sending a packet for decoding.";
                    continue;
                }

                ret = avcodec_receive_frame(codecCtx, pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    continue;
                } else if (ret < 0) {
                    qDebug() << "Error during video decoding.";
                    continue;
                }

                // 转换像素格式  
                int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, pFrame->width, pFrame->height, 32);  
                uint8_t *buffer = (uint8_t *)av_malloc(numBytes);  
                int linesize[1] = {av_image_get_linesize(AV_PIX_FMT_BGR24, pFrame->width, 0)};  
                sws_scale(sws_ctx, (const uint8_t * const *)pFrame->data, pFrame->linesize, 0, pFrame->height, &buffer, linesize);  

                // 创建 OpenCV Mat 并保存图片  
                mat = cv::Mat(pFrame->height, pFrame->width, CV_8UC3, buffer, linesize[0]);
                //cv::imwrite("frame.jpg", mat);

                emit sigGetFrame(mat);
                // 清理
                av_free(buffer);
            }  
        }  
    }
    if (sws_ctx) {
        sws_freeContext(sws_ctx);
    }
}

void MyCamera::stopCamera() {
    isRunning = false;
    this->wait();
}

cv::Mat MyCamera::getMat(){
    return mat;
}

void MyCamera::setLabel(QLabel *la){
    test = la;
}
