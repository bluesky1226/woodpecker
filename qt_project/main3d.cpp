#include <QApplication>
#include <QMainWindow>
#include <QtDataVisualization/Q3DScatter>

// using namespace QtDataVisualization;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create the graph
    Q3DScatter *scatter = new Q3DScatter();

    // Wrap it
    QWidget *container = QWidget::createWindowContainer(scatter);

    // Create a simple window to hold it
    QMainWindow mainWindow;
    mainWindow.setCentralWidget(container);
    mainWindow.resize(800, 600);
    mainWindow.setWindowTitle("3D Test");

    // Add one single point so it's not empty
    QScatter3DSeries *series = new QScatter3DSeries();
    QScatterDataArray data;
    data << QVector3D(0.5f, 0.5f, 0.5f);
    series->dataProxy()->addItems(data);
    scatter->addSeries(series);

    mainWindow.show(); // <--- Is this line present?

    return app.exec();
}