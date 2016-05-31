#include <QtGui>
#include <QApplication>
#include "mainwindow.h"
#include <QPixmap>
#include <QMessageBox>
#include <QTimer>




int main(int argc, char *argv[])
{

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QApplication::setGraphicsSystem("raster");
#endif

    QApplication a(argc, argv);

    QString sDir = QCoreApplication::applicationDirPath();

        a.addLibraryPath(sDir+"/plugins");

    MainWindow w;
    w.show();
    
    return a.exec();
}

