#include "datadisplay.h"
#include "ui_datadisplay.h"

DataDisplay::DataDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataDisplay)
{
    ui->setupUi(this);

    QFile outfile;
    outfile.setFileName("Test_Station_Data_log.txt");
    outfile.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
    setWindowTitle(tr("Data display"));
}

DataDisplay::~DataDisplay()
{
    delete ui;
}

void DataDisplay::dispdata(const QString &text)
{
    QFile outfile;
    outfile.setFileName("Test_Station_Data_log.txt");
    outfile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    setWindowTitle(tr("Data display"));
    QTime time = QTime::currentTime();
    QTextStream out(&outfile);

    ui->textEdit->append(time.toString() + "\t" + text);
    out << ui->textEdit->toPlainText()<< endl;
}
