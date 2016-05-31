#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <iostream>

#include "qcustomplot.h"
#include "datadisplay.h"

namespace Ui {
class MainWindow;
}

class QTimer;
class QextSerialPort;
class QextSerialEnumerator;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

public slots:
    void run();
    void startread();
    void startread_off();
    void disconnectClient();
    void stopplot();
    void resetfun();
    void encoderfun();
    void offloaderfun();
    void offloaderfun_1();
    void vcmfun();
    void resetdrivefun();
    void showdatadisp();
    void saveplot_fun();
    void about_fun();

private slots:
    void realdataslot();
    void realdataslot2();
    void realdataslot_off();
    void screenshot();

private Q_SLOTS:
    void onPortNameChanged(const QString &name);
    void onBaudRateChanged(int idx);
    void onParityChanged(int idx);
    void onDataBitsChanged(int idx);
    void onStopBitsChanged(int idx);
    void onQueryModeChanged(int idx);
    void onTimeoutChanged(int val);

    void onOpenCloseButtonClicked();
    void onSendButtonClicked();
    void onReadyRead();
    void onPortAddedOrRemoved();


private:
    Ui::MainWindow *ui;
    QString dName;
    QTimer dTimer;
    QTimer *timer;
    QCPItemTracer *itemphasetracer;
    int currentdIndex;
    QextSerialPort *port;
    QextSerialEnumerator *enumerator;


public:
    DataDisplay *datadisp;
    QTcpSocket *socket;
    QTextStream *ts;
    double df, df_1, df_2, df_3;
    double offcount;
};

#endif // MAINWINDOW_H
