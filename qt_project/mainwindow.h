#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h" // Include the plotting library
#include "adxl345.h"
#include <QVector>

#define QT_SAMPLE_RATE  1500 // HZ

using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    double startTime;
    int wsize;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // This is the function you created earlier
    void updatePlotData();
    void setupRealTimePlot(QCustomPlot *customPlot);
    void set_plot_ind(QCustomPlot *customPlot);
    void set_plot_limit(const vector<float> vdata);

private:
    ADXL345 *accel = nullptr;
    QVector<double> vtime, vx, vy, vz;
    QCustomPlot *customPlot; // The pointer to your graph widget
};

#endif // MAINWINDOW_H