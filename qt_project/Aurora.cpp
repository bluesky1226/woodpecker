
#include "Aurora.h"
#include <QtDataVisualization>
#include <QVBoxLayout>
#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/qscatterdataproxy.h>
#include <QtDataVisualization/qscatter3dseries.h>

// using namespace QtDataVisualization;

Aurora::Aurora(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Qt 3D Scatter Plot Example");
    resize(800, 600);

    // 1. Create the 3D Scatter Object
    Q3DScatter *scatter = new Q3DScatter();

    // 2. Create a container to wrap the QWindow into a QWidget
    QWidget *container = QWidget::createWindowContainer(scatter);
    setCentralWidget(container);

    // 3. Create a Data Series and Proxy
    QScatter3DSeries *series = new QScatter3DSeries();
    QScatterDataArray data;
    // Generate 50 random points in 3D space
    for (float i = 0.0f; i < 5.0f; i += 0.1f) {
        float x = i;
        float y = qSin(i) * 2.0f;
        float z = qCos(i) * 2.0f;
        data << QVector3D(x, y, z);
    }
    series->dataProxy()->addItems(data);

    // 4. Customize Appearance
    // series->setMesh(QAbstract3DSeries::MeshSphere); // Point shape
    series->setMesh(QAbstract3DSeries::MeshPoint);
    series->setBaseColor(Qt::blue);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel, @yTitle: @yLabel, @zTitle: @zLabel"));

    // 5. Configure Axes
    scatter->addSeries(series);
    scatter->axisX()->setTitle("X Axis");
    scatter->axisY()->setTitle("Y Axis");
    scatter->axisZ()->setTitle("Z Axis");

    // 6. Interaction settings
    scatter->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftLow);
    scatter->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFrontHigh);
    scatter->setActiveInputHandler(new Q3DInputHandler());
}

Aurora::~Aurora()
{
}
