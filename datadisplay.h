#ifndef DATADISPLAY_H
#define DATADISPLAY_H

#include <QDialog>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <iostream>

namespace Ui {
class DataDisplay;
}

class DataDisplay : public QDialog
{
    Q_OBJECT
    
public:
    explicit DataDisplay(QWidget *parent = 0);
    ~DataDisplay();

    void dispdata(const QString &text);
    
private:
    Ui::DataDisplay *ui;
    QFile outfile;
};

#endif // DATADISPLAY_H
