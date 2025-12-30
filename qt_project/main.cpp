
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv); // Manages GUI settings and event loop

    MainWindow w;               // Creates your window object
    w.show();                   // Makes the window visible

    return a.exec();            // Starts the main event loop
}
