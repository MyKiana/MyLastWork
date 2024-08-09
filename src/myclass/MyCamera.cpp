#include "MyCamera.h"



void MyCamera::openCamera(){

    // 注册所有文件格式和编解码器  
    avformat_network_init();

    avdevice_register_all();

    AVFormatContext *pFormatCtx = NULL;  
    if (avformat_open_input(&pFormatCtx, "/dev/video0", NULL, NULL) != 0) {  
        // 错误处理  
        qDebug() << "Could not open video device.";
        return ;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {  
        qDebug() << "Could not find stream information.";  
        avformat_close_input(&pFormatCtx);  
        return ;  
    }  

    int videoStreamIndex = -1;  
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
    qDebug() << "codec_id : " << codecpar->codec_id;
    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        qDebug() << "Failed to find codec for video stream.";
        return;
    }

    // 初始化解码器上下文
    AVCodecContext *codecCtx = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(codecCtx, codecpar) < 0) {
        qDebug() << "Failed to copy codec parameters.";
        return;
    }
    if (avcodec_open2(codecCtx, codec, NULL) < 0) { 
        qDebug() << "Failed to open codec.";
        return;
    }
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
    qDebug() << "pix_fmt : " << codecCtx->pix_fmt;
  
    // 这里可以进一步设置解码器上下文等，但在这个示例中我们只是读取帧  
    AVPacket *pPacket = av_packet_alloc();  
    // if(pPacket == NULL){
    //     qDebug() << "pPacket is NULL";
    // }
    // 
    AVFrame *pFrame = av_frame_alloc();  

    // 读取帧  
    //av_read_frame(pFormatCtx, pPacket);
    while (av_read_frame(pFormatCtx, pPacket) >= 0) {  
        if (pPacket->stream_index == videoStreamIndex) {  

            // 将AVPacket数据送入解码器
            int ret = avcodec_send_packet(codecCtx, pPacket);
            if (ret < 0) {
                qDebug() << "Error sending a packet for decoding.";
                av_packet_unref(pPacket);
                return;
            }

            ret = avcodec_receive_frame(codecCtx, pFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                av_frame_free(&pFrame);
                return;
            } else if (ret < 0) {
                qDebug() << "Error during video decoding.";
                av_frame_free(&pFrame);
                av_packet_unref(pPacket);
                return;
            }

            // // 将解码后的帧保存为图片
            // saveFrameAsImage(frame);
            // 转换像素格式
            int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, pFrame->width, pFrame->height, 1);
            uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
            sws_scale(
                sws_ctx,
                (const uint8_t * const *)pFrame->data,
                pFrame->linesize,
                0,
                pFrame->height,
                &buffer,
                pFrame->linesize
            );

            // 将转换后的帧保存为图片
            cv::Mat mat(pFrame->height, pFrame->width, CV_8UC3, buffer);
            qDebug() << pFrame->width << "    :     " << mat.cols;
            cv::imwrite("frame.jpg", mat);

            // 清理
            av_free(buffer);
        }  
    }  
  
    // 释放资源  
    av_frame_free(&pFrame);  
    av_packet_free(&pPacket);  
    avformat_close_input(&pFormatCtx);  
}

