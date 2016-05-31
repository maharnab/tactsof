#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qextserialport.h"
#include "qextserialenumerator.h"

#include "about.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    offcount = 0;
    datadisp = new DataDisplay(this);
//    this->setWindowFlags(Qt::WindowStaysOnTopHint);
//    this->setAttribute(Qt::WA_TranslucentBackground, true);

    setWindowTitle(tr("TMT Actuator Test Station Software"));
    ui->enccomboBox->addItem("10 seconds");
    ui->enccomboBox->addItem("25 seconds");
    ui->enccomboBox->addItem("50 seconds");
    ui->enccomboBox->addItem("100 seconds");

    ui->offcomboBox->addItem("1 mm");
    ui->offcomboBox->addItem("3 mm");
    ui->offcomboBox->addItem("6 mm");

    ui->vcmcomboBox->addItem("20 seconds");
    ui->vcmcomboBox->addItem("40 seconds");
    ui->vcmcomboBox->addItem("80 seconds");
    ui->vcmcomboBox->addItem("100 seconds");

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
//    ui->customPlot->setBackground(Qt::transparent);
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setFont(QFont("Helvetica", 7));
    ui->customPlot->axisRect()->setBackground(Qt::darkGray);
    ui->customPlot->axisRect()->setupFullAxesBox();

    ui->customPlot->addGraph(); // blue line
    ui->customPlot->graph(0)->setName("Encoder Count vs. Time");
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->customPlot->graph(0)->setAntialiasedFill(false);

    ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->customPlot->xAxis->setAutoTickStep(true);
    ui->customPlot->xAxis->setTickStep(1);

    ui->customPlot_1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    ui->customPlot_1->legend->setVisible(true);
    ui->customPlot_1->legend->setFont(QFont("Helvetica", 7));
    ui->customPlot_1->axisRect()->setBackground(Qt::darkGray);
    ui->customPlot_1->axisRect()->setupFullAxesBox();

    ui->customPlot_1->addGraph(); // blue line
    ui->customPlot_1->graph(0)->setName("Sensor Data vs. Time");
    ui->customPlot_1->graph(0)->setPen(QPen(Qt::green));
    ui->customPlot_1->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->customPlot_1->graph(0)->setAntialiasedFill(false);

    ui->customPlot_1->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot_1->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->customPlot_1->xAxis->setAutoTickStep(true);

    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
    ui->portBox->addItem(info.portName);

    ui->portBox->setEditable(true);

    ui->baudRateBox->addItem("9600", BAUD9600);
    ui->baudRateBox->addItem("115200", BAUD115200);
    ui->baudRateBox->setCurrentIndex(2);

    ui->parityBox->addItem("NONE", PAR_NONE);
    ui->parityBox->addItem("ODD", PAR_ODD);
    ui->parityBox->addItem("EVEN", PAR_EVEN);

    ui->dataBitsBox->addItem("5", DATA_5);
    ui->dataBitsBox->addItem("6", DATA_6);
    ui->dataBitsBox->addItem("7", DATA_7);
    ui->dataBitsBox->addItem("8", DATA_8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->stopBitsBox->addItem("1", STOP_1);
    ui->stopBitsBox->addItem("2", STOP_2);

    ui->queryModeBox->addItem("POLLING", QextSerialPort::Polling);
    ui->queryModeBox->addItem("EVENT DRIVEN", QextSerialPort::EventDriven);

    timer = new QTimer(this);
    timer->setInterval(40);

    PortSettings settings = {BAUD115200, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    port = new QextSerialPort(ui->portBox->currentText(), settings, QextSerialPort::Polling);

    enumerator = new QextSerialEnumerator(this);
    enumerator->setUpNotifications();

    connect(ui->connect_Button, SIGNAL(clicked()), SLOT(run()));
    connect(ui->disconnect_Button, SIGNAL(clicked()), SLOT(disconnectClient()));
    connect(ui->stop_Button, SIGNAL(clicked()), SLOT(stopplot()));
    connect(ui->reset_window_Button, SIGNAL(clicked()), SLOT(resetfun()));
    connect(ui->enc_test_run_Button, SIGNAL(clicked()), SLOT(encoderfun()));
    connect(ui->enc_test_run_Button, SIGNAL(clicked()), SLOT(showdatadisp()));
    connect(ui->off_up_Button, SIGNAL(clicked()), SLOT(offloaderfun()));
    connect(ui->off_down_Button, SIGNAL(clicked()), SLOT(offloaderfun_1()));
    connect(ui->vcm_test_run_Button, SIGNAL(clicked()), SLOT(vcmfun()));
    connect(ui->vcm_test_run_Button, SIGNAL(clicked()), SLOT(showdatadisp()));
    connect(ui->reset_drive_board_Button, SIGNAL(clicked()), SLOT(resetdrivefun()));
    connect(ui->save_plot_Button, SIGNAL(clicked()), SLOT(saveplot_fun()));

    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)), SLOT(onBaudRateChanged(int)));
    connect(ui->parityBox, SIGNAL(currentIndexChanged(int)), SLOT(onParityChanged(int)));
    connect(ui->dataBitsBox, SIGNAL(currentIndexChanged(int)), SLOT(onDataBitsChanged(int)));
    connect(ui->stopBitsBox, SIGNAL(currentIndexChanged(int)), SLOT(onStopBitsChanged(int)));
    connect(ui->portBox, SIGNAL(editTextChanged(QString)), SLOT(onPortNameChanged(QString)));

    connect(ui->openCloseButton, SIGNAL(clicked()), SLOT(onOpenCloseButtonClicked()));
    connect(ui->sendButton, SIGNAL(clicked()), SLOT(onSendButtonClicked()));
    connect(ui->aboutButton, SIGNAL(clicked()), SLOT(about_fun()));

    connect(timer, SIGNAL(timeout()), SLOT(onReadyRead()));
    connect(port, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(enumerator, SIGNAL(deviceDiscovered(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
    connect(enumerator, SIGNAL(deviceRemoved(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
}

void MainWindow::run()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.192.192.211", 2234, QIODevice::ReadWrite);
    ui->statusBar->showMessage("Connected...");
}

void MainWindow::disconnectClient()
{
    ui->statusBar->showMessage("Disonnected...");
    socket->close();
    dTimer.stop();
}

void MainWindow::stopplot()
{
     dTimer.stop();
}

void MainWindow::resetfun()
{

    ui->customPlot->graph(0)->clearData();
    ui->customPlot->replot();

    ui->customPlot_1->graph(0)->clearData();
    ui->customPlot_1->replot();
}

void MainWindow::encoderfun()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.192.192.211", 2234, QIODevice::ReadWrite);
    if(ui->enccomboBox->currentText()=="10 seconds")
    {
        ui->statusBar->showMessage("Encoder test is running for 10 seconds...");
        socket->write("e");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->enccomboBox->currentText()=="25 seconds")
    {
        ui->statusBar->showMessage("Encoder test is running for 25 seconds...");
        socket->write("f");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->enccomboBox->currentText()=="50 seconds")
    {
        ui->statusBar->showMessage("Encoder test is running for 50 seconds...");
        socket->write("g");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->enccomboBox->currentText()=="100 seconds")
    {
        ui->statusBar->showMessage("Encoder test is running for 100 seconds...");
        socket->write("h");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    connect(socket, SIGNAL(readyRead()), SLOT(startread()));
}

void MainWindow::offloaderfun()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.192.192.211", 2234, QIODevice::ReadWrite);
    if(ui->offcomboBox->currentText()=="1 mm")
    {
        ui->statusBar->showMessage("Off Loader is going up by 1 mm...");
        socket->write("u");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->offcomboBox->currentText()=="3 mm")
    {
        ui->statusBar->showMessage("Off Loader is going up by 3 mm...");
        socket->write("t");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->offcomboBox->currentText()=="6 mm")
    {
        ui->statusBar->showMessage("Off Loader is going up by 6 mm...");
        socket->write("s");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    connect(socket, SIGNAL(readyRead()), SLOT(startread_off()));
}

void MainWindow::offloaderfun_1()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.192.192.211", 2234, QIODevice::ReadWrite);
    if(ui->offcomboBox->currentText()=="1 mm")
    {
        ui->statusBar->showMessage("Off Loader is going down by 1 mm...");
        socket->write("d");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->offcomboBox->currentText()=="3 mm")
    {
        ui->statusBar->showMessage("Off Loader is going down by 3 mm...");
        socket->write("c");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->offcomboBox->currentText()=="6 mm")
    {
        ui->statusBar->showMessage("Off Loader is going down by 6 mm...");
        socket->write("b");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    connect(socket, SIGNAL(readyRead()), SLOT(startread_off()));
}

void MainWindow::vcmfun()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.192.192.211", 2234, QIODevice::ReadWrite);
    if(ui->vcmcomboBox->currentText()=="20 seconds")
    {
        ui->statusBar->showMessage("VCM test is running for 20 seconds...");
        socket->write("v");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->vcmcomboBox->currentText()=="40 seconds")
    {
        ui->statusBar->showMessage("VCM test is running 40 seconds...");
        socket->write("w");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->vcmcomboBox->currentText()=="80 seconds")
    {
        ui->statusBar->showMessage("VCM test is running 80 seconds...");
        socket->write("x");
        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    if(ui->vcmcomboBox->currentText()=="100 seconds")
    {
        ui->statusBar->showMessage("VCM test is running 100 seconds...");
        socket->write("y");

        QString command3 = "/ftest/vcm_test_5avs20sec \r";
        const char *command4;
        command4 = command3.toLocal8Bit().data();
        port->write(command4);
    }
    connect(socket, SIGNAL(readyRead()), SLOT(startread()));
}

void MainWindow::resetdrivefun()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.192.192.211", 2234, QIODevice::ReadWrite);
    ui->statusBar->showMessage("Resetting drive board...");
    socket->write("r");

    QString command3 = "/ftest/vcm_test_5avs20sec \r";
    const char *command4;
    command4 = command3.toLocal8Bit().data();
    port->write(command4);

    offcount = 0;
}

void MainWindow::showdatadisp()
{
    datadisp->exec();
}

void MainWindow::saveplot_fun()
{
    QTimer::singleShot(1000, this, SLOT(screenshot()));
}

void MainWindow::about_fun()
{
    About mAbout(this);
    mAbout.setModal(true);
    mAbout.exec();
}

void MainWindow::startread()
{
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->customPlot_1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    QString d ;
    d = socket->readAll();
    df = d.toDouble();
    if(df > 10000000)
    {
        df_1 = df;
        QString valueAsString = QString::number(df_1);
        datadisp->dispdata(valueAsString);
        connect(&dTimer, SIGNAL(timeout()), this, SLOT(realdataslot2()));
        dTimer.start(0);
    }
    else
    {
        df_2 = df;
        QString valueAsString = QString::number(df_2);
        datadisp->dispdata(valueAsString);
        connect(&dTimer, SIGNAL(timeout()), this, SLOT(realdataslot()));
        dTimer.start(0);
    }
}

void MainWindow::startread_off()
{
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    QString d ;
    d = socket->readAll();
    df = d.toDouble();

    df_3 = df;
    connect(&dTimer, SIGNAL(timeout()), this, SLOT(realdataslot_off()));
    dTimer.start(0);
}

void MainWindow::realdataslot()
{
    // calculate two new data points:
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
        double key = 0;
#else
        double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
        static double lastPointKey = 0;

        if (key-lastPointKey > 0.01 ) // at most add point every 10 ms
        {
//            ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

            double value0 = df_2;

            if (ui->customPlot->yAxis->range().size() < (2*fabs(value0)) ||  ui->customPlot->yAxis->range().size() > (2*fabs(value0)))
            {
                ui->customPlot->yAxis->setRange(ui->customPlot->yAxis->range().center(), (2*fabs(value0)) + (2 * fabs(value0) * 0.25), Qt::AlignCenter);
            }

            // add data to lines:
            ui->customPlot->graph(0)->addData(key, value0);
            ui->customPlot->graph(0)->rescaleValueAxis(true);

            lastPointKey = key;
        }
        ui->customPlot->xAxis->setRange(key+1.0, 15, Qt::AlignRight);
        ui->customPlot->replot();
}

void MainWindow::realdataslot_off()
{
    // calculate two new data points:
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
        double key = 0;
#else
        double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
        static double lastPointKey = 0;

        if (key-lastPointKey > 0.01 ) // at most add point every 10 ms
        {
//            ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

            offcount = offcount+ df_3;
            df_3 = 0;
            double value0 = offcount;

            if (ui->customPlot->yAxis->range().size() < (2*fabs(value0)) ||  ui->customPlot->yAxis->range().size() > (2*fabs(value0)))
            {
                ui->customPlot->yAxis->setRange(ui->customPlot->yAxis->range().center(), (2*fabs(value0)) + (2 * fabs(value0) * 0.25), Qt::AlignCenter);
            }

            // add data to lines:
            ui->customPlot->graph(0)->addData(key, value0);
            ui->customPlot->graph(0)->rescaleValueAxis(true);

            lastPointKey = key;
        }
        ui->customPlot->xAxis->setRange(key+1.0, 15, Qt::AlignRight);
        ui->customPlot->replot();
}

void MainWindow::realdataslot2()
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    double key = 0;
#else
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
    static double lastPointKey = 0;

    if (key-lastPointKey > 0.1) // at most add point every 10 ms
    {
        double value1 = df_1 - 10000000;

        if (ui->customPlot_1->yAxis->range().size() < (2*fabs(value1)) || ui->customPlot_1->yAxis->range().size() > (2*fabs(value1)))
        {
            ui->customPlot_1->yAxis->setRange(ui->customPlot_1->yAxis->range().center(), (2*fabs(value1)) + (2 * fabs(value1) * 0.25), Qt::AlignCenter);
        }

        ui->customPlot_1->graph(0)->addData(key, value1);
        ui->customPlot_1->graph(0)->rescaleValueAxis(true);

        lastPointKey = key;
    }
    ui->customPlot_1->xAxis->setRange(key+1.0, 15, Qt::AlignRight);
    ui->customPlot_1->replot();
}

void MainWindow::screenshot()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QPixmap pm = QPixmap::grabWindow(qApp->desktop()->winId(), this->x()+2, this->y()+2, this->frameGeometry().width()-4, this->frameGeometry().height()-4);
#else
  QPixmap pm = qApp->primaryScreen()->grabWindow(qApp->desktop()->winId(), this->x()+2, this->y()+2, this->frameGeometry().width()-4, this->frameGeometry().height()-4);
#endif
  dName = "Test_Station_screenshot";
  QString fileName = dName.toLower()+".png";
  fileName.replace(" ", "");
  pm.save("./"+fileName);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete port;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::onPortNameChanged(const QString & /*name*/)
{
    if (port->isOpen()) {
        port->close();
    }
}

void MainWindow::onBaudRateChanged(int idx)
{
    port->setBaudRate((BaudRateType)ui->baudRateBox->itemData(idx).toInt());
}

void MainWindow::onParityChanged(int idx)
{
    port->setParity((ParityType)ui->parityBox->itemData(idx).toInt());
}

void MainWindow::onDataBitsChanged(int idx)
{
    port->setDataBits((DataBitsType)ui->dataBitsBox->itemData(idx).toInt());
}

void MainWindow::onStopBitsChanged(int idx)
{
    port->setStopBits((StopBitsType)ui->stopBitsBox->itemData(idx).toInt());
}

void MainWindow::onQueryModeChanged(int idx)
{
    port->setQueryMode((QextSerialPort::QueryMode)ui->queryModeBox->itemData(idx).toInt());
}

void MainWindow::onTimeoutChanged(int val)
{
    port->setTimeout(val);
}

void MainWindow::onOpenCloseButtonClicked()
{
    if (!port->isOpen())
    {
        port->setPortName(ui->portBox->currentText());
        port->open(QIODevice::ReadWrite);
    }
    else
    {
        port->close();
    }

    if (port->isOpen() && port->queryMode() == QextSerialPort::Polling)
        timer->start();
    else
        timer->stop();
}

void MainWindow::onSendButtonClicked()
{
    if (port->isOpen() && !ui->plainTextEdit_2->toPlainText().isEmpty())
        port->write(ui->plainTextEdit_2->toPlainText().toLatin1());
}

void MainWindow::onReadyRead()
{
    if (port->bytesAvailable())
    {
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        ui->plainTextEdit->insertPlainText(QString::fromLatin1(port->readAll()));
    }
}

void MainWindow::onPortAddedOrRemoved()
{
    QString current = ui->portBox->currentText();

    ui->portBox->blockSignals(true);
    ui->portBox->clear();
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        ui->portBox->addItem(info.portName);

    ui->portBox->setCurrentIndex(ui->portBox->findText(current));

    ui->portBox->blockSignals(false);
}
