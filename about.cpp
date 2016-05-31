#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    //    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);

        setWindowTitle(tr("About"));
}

About::~About()
{
    delete ui;
}
