#include "Microscope.h"

Microscope::Microscope(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_Microscope)
{
    qRegisterMetaType< cv::Mat >("cv::Mat");
    ui->setupUi(this);
    initCamera();
    initSig();
}

Microscope::~Microscope()
{
    delete ui; 
}

void Microscope::initSig(){
    connect(&myCmaera, &MyCamera::sigGetFrame, this, &Microscope::receiveFrame);
}

void Microscope::initCamera()
{
    myCmaera.openCamera("/dev/video1");
}

void Microscope::closeEvent(QCloseEvent *event){
    QWidget::closeEvent(event);
    qDebug() << "close";
    myCmaera.stopCamera();
}

void Microscope::receiveFrame(cv::Mat fr){
    qDebug() << __FUNCTION__;
    frame = std::move(fr);
    cv::Mat test;
    cv::cvtColor(frame, test, cv::COLOR_BGR2RGB);
    QImage img = QImage((const unsigned char*)(test.data), test.cols, test.rows, test.step,QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(img));
    ui->label->resize(ui->label->pixmap()->size());
}