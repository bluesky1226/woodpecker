
#include "Aurora.h"
#include <QtDataVisualization>
#include <QVBoxLayout>

// using namespace QtDataVisualization;

Aurora::Aurora(QWidget *parent) : QMainWindow(parent)
{
    // 1. Create the 3D Scatter Graph
    Q3DScatter *scatterGraph = new Q3DScatter();
    
    // 2. Create a container widget (this bridge is required for 3D)
    QWidget *container = QWidget::createWindowContainer(scatterGraph);
    container->setMinimumSize(QSize(1024, 768));
    setCentralWidget(container);

    // 3. Configure Axis Titles
    scatterGraph->axisX()->setTitle("X-Axis");
    scatterGraph->axisY()->setTitle("Y-Axis (Height)");
    scatterGraph->axisZ()->setTitle("Z-Axis");
    scatterGraph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftMedium);

    // 4. Create a Data Series
    QScatter3DSeries *series = new QScatter3DSeries();
    QScatterDataArray *dataArray = new QScatterDataArray;

    // 5. Generate Random 3D Data (X, Y, Z)
    for (float i = 0.0f; i < 10.0f; i += 0.1f) {
        float x = i;
        float y = qSin(i); // Create a wave pattern
        float z = qCos(i);
        *dataArray << QVector3D(x, y, z);
    }

    // 6. Push data to the graph
    series->dataProxy()->addItems(*dataArray);
    scatterGraph->addSeries(series);

    // 7. Styling (Optional)
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel, @yTitle: @yLabel, @zTitle: @zLabel"));
    scatterGraph->activeTheme()->setType(Q3DTheme::ThemeEbony); // Dark theme
}

Aurora::~Aurora()
{
}
