
#include "Aurora.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    // This tells the Pi to use the GPU driver directly
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    
    // Sometimes the Pi needs to share OpenGL contexts
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication a(argc, argv); // Manages GUI settings and event loop

    Aurora w;               // Creates your window object
    w.show();                   // Makes the window visible

    return a.exec();            // Starts the main event loop
}
