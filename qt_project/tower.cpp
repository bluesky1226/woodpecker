
#include "Aurora.h"
#include <QApplication>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QMainWindow>


class SurfaceWindow : public QMainWindow {
public:
    SurfaceWindow() {
        // 1. Initialize Graph
        Q3DSurface *surface = new Q3DSurface();
        QWidget *container = QWidget::createWindowContainer(surface);
        setCentralWidget(container);

        // 2. Create Data Proxy and Series
        QSurfaceDataProxy *proxy = new QSurfaceDataProxy();
        QSurface3DSeries *series = new QSurface3DSeries(proxy);

        // 3. Generate Data (Grid of Rows and Columns)
        int sampleCountX = 50; // Rows
        int sampleCountZ = 50; // Columns

        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        dataArray->reserve(sampleCountZ);

        for (int z = 0; z < sampleCountZ; z++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
            float zVal = (float)z / 10.0f;

            for (int x = 0; x < sampleCountX; x++) {
                float xVal = (float)x / 10.0f;
                float yVal = qSin(xVal) * qCos(zVal); // The "Height"

                // Set the point (X, Y, Z)
                (*newRow)[x].setPosition(QVector3D(xVal, yVal, zVal));
            }
            *dataArray << newRow;
        }

        proxy->resetArray(dataArray);

        // 4. Appearance: Add a Color Gradient
        QLinearGradient gr;
        gr.setColorAt(0.0, Qt::blue);   // Low points
        gr.setColorAt(0.5, Qt::green);  // Middle points
        gr.setColorAt(1.0, Qt::red);    // High points
        series->setBaseGradient(gr);
        series->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

        // 5. Final Setup
        surface->addSeries(series);
        surface->axisX()->setTitle("X (Width)");
        surface->axisY()->setTitle("Y (Height)");
        surface->axisZ()->setTitle("Z (Depth)");

        // Optimize for Raspberry Pi
        surface->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    }
};


int main(int argc, char *argv[]) {
    // This tells the Pi to use the GPU driver directly
    qputenv("QT_XCB_GL_INTEGRATION", "xcb_egl");
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    
    // Sometimes the Pi needs to share OpenGL contexts
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication a(argc, argv); // Manages GUI settings and event loop

    // Aurora w;               // Creates your window object
    SurfaceWindow w;
    w.show();                   // Makes the window visible

    return a.exec();            // Starts the main event loop
}
