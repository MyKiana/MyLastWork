#pragma once
#include "ui_Microscope.h"
#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include "src/myclass/MyCamera.h"


class Microscope : public QMainWindow {
    Q_OBJECT
    
public:
    Microscope(QWidget* parent = nullptr);
    ~Microscope();
    void initCamera();

private:
    Ui_Microscope* ui;

    MyCamera myCmaera;

    cv::Mat frame;
};