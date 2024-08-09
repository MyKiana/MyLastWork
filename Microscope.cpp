#include "Microscope.h"

Microscope::Microscope(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_Microscope)
{
    ui->setupUi(this);
    initCamera();
}

Microscope::~Microscope()
{
    delete ui; 
}

void Microscope::initCamera()
{
    myCmaera.openCamera();
}