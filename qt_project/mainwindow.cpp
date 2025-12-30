
#include "qcustomplot.h"
#include "mainwindow.h"
#include "adxl345.h"
#include <cmath>
#include <QTimer>
#include <QMargins>
#include <QVector>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->resize(1024, 768);
    customPlot = new QCustomPlot(this);
    setCentralWidget(customPlot);
    set_plot_ind(customPlot);
}

MainWindow::~MainWindow()
{
}

void MainWindow::updatePlotData()
{

}

void MainWindow::set_plot_limit(const vector<float> vdata)
{
    float v_min, v_max, v_avg, v_sum;
    int v_min_border, v_max_border;
    string stp;

    // for (int i = 0; i < vdata.size(); i++)
    // {
    //     printf("vdata: %.4f\n", vdata[i]);
    // }
    auto result = std::minmax_element(vdata.begin(), vdata.end());
    v_min = *result.first;
    v_max = *result.second;
    v_sum = std::accumulate(vdata.begin(), vdata.end(), 0.0);
    // printf("v_sum: %.4f\n", v_sum);
    v_avg = v_sum / vdata.size();
    // printf("v_avg: %.4f\n", v_avg);

    if (abs(v_avg - v_max) < 1)
    {
        v_max = v_avg + 1.5;
    }
    else
    {
        v_max = v_avg + abs(v_avg - v_max)*1.5;
    }

    if (abs(v_avg - v_min) < 1)
    {
        v_min = v_avg - 1.5;
    }
    else
    {
        v_min = v_avg - abs(v_avg - v_min)*1.5;
    }

    // printf("v_min: %.4f, v_max: %.4f\n", v_min, v_max);
    if (v_min >= 0)
    {
        v_min_border = std::ceil(v_min);
    }
    else
    {
        v_min_border = std::floor(v_min);
    }

    if (v_max >= 0)
    {
        v_max_border = std::ceil(v_max);
    }
    else
    {
        v_max_border = std::floor(v_max);
    }

    // printf("min_b: %d, max_b: %d\n", v_min_border, v_max_border);
    customPlot->yAxis->setRange(v_min_border, v_max_border);
}

// Inside your Window Constructor:
void MainWindow::setupRealTimePlot(QCustomPlot *customPlot) {
    customPlot->addGraph(); // Blue line for X-axis
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setName("X-Axis Data");

    customPlot->addGraph(); 
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setName("Y-Axis Data");

    customPlot->addGraph(); 
    customPlot->graph(2)->setPen(QPen(Qt::green));
    customPlot->graph(2)->setName("Z-Axis Data");

    customPlot->xAxis->setLabel("Time (s)");
    customPlot->yAxis->setLabel("Acceleration");
    // customPlot->yAxis->setRange(-3, 3);

    int rtc = -1;
    accel = new ADXL345("/dev/spidev1.2", -1);
    rtc = accel->check_linux_device();
    if (rtc < 0) {
        printf("ADXL345 linux device failed!!\n");
        return;
    }

    rtc = accel->check_adxl345_link();
    if (rtc < 0) {
        printf("ADXL345 link failed!!\n");
        return;
    }

    if (rtc >= 0) {
        printf("ADXL345 init successfully\n");
    }

    // accel = new ADXL345("/dev/spidev0.0", 5000000);
    // accel->check_device();
    accel->initialization();

    // Setup a timer to update data
    startTime = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
    QTimer *dataTimer = new QTimer(this);
    connect(dataTimer, &QTimer::timeout, this, [=]() {
        float x, y, z;
        double currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
        double diff_time = currentTime - this->startTime;

        // 1. Get your ADXL345 data here
        this->accel->readXYZ(x, y, z);
        // printf("%.4f, %.4f, %.4f\n", x, y, z);

        this->vx.push_back(x);
        this->vy.push_back(y);
        this->vz.push_back(z);

        // Keep only the last MAX_POINTS
        if (this->vx.size() > MAX_POINTS)
        {
            this->vx.erase(this->vx.begin());
            this->vy.erase(this->vy.begin());
            this->vz.erase(this->vz.begin());
        }
        if (this->wsize > 500) {
            this->startTime = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
            this->wsize = 0;
        }

        // 2. Add data to the graph
        customPlot->graph(0)->addData(diff_time, x);
        customPlot->graph(1)->addData(diff_time, y);
        customPlot->graph(2)->addData(diff_time, z);
        // customPlot->graph(0)->rescaleValueAxis(true);
        customPlot->rescaleAxes(true);

        // 3. Keep the "window" moving (show last 8 seconds)
        customPlot->xAxis->setRange(diff_time, 20, Qt::AlignRight);
        customPlot->replot();
    });

    dataTimer->start(2);
}

void MainWindow::set_plot_ind(QCustomPlot *customPlot) {
    int ndp = 3;
    int rtb = 3;
    int rtc = 0;

    accel = new ADXL345("/dev/spidev1.2", -1);
    rtc = accel->check_linux_device();
    if (rtc < 0) {
        printf("ADXL345 linux device failed!!\n");
        return;
    }

    rtc = accel->check_adxl345_link();
    if (rtc < 0) {
        printf("ADXL345 link failed!!\n");
        return;
    }

    if (rtc >= 0) {
        printf("ADXL345 init successfully\n");
    }

    accel->initialization();

    // 1. Prepare the layout
    customPlot->plotLayout()->clear();

    // Create 3 Rects (Charts)
    QCPAxisRect *rectX = new QCPAxisRect(customPlot);
    QCPAxisRect *rectY = new QCPAxisRect(customPlot);
    QCPAxisRect *rectZ = new QCPAxisRect(customPlot);
    QCPAxisRect *rectXY = new QCPAxisRect(customPlot);
    QCPAxisRect *rectXZ = new QCPAxisRect(customPlot);
    QCPAxisRect *rectYZ = new QCPAxisRect(customPlot);

    rectX->axis(QCPAxis::atBottom)->setRange(0, rtb);
    rectY->axis(QCPAxis::atBottom)->setRange(0, rtb);
    rectZ->axis(QCPAxis::atBottom)->setRange(0, rtb);

    rectX->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectY->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectZ->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectXY->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectXZ->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectYZ->axis(QCPAxis::atLeft)->setNumberFormat("f");

    rectX->axis(QCPAxis::atBottom)->setNumberFormat("f");
    rectY->axis(QCPAxis::atBottom)->setNumberFormat("f");
    rectZ->axis(QCPAxis::atBottom)->setNumberFormat("f");
    rectXY->axis(QCPAxis::atBottom)->setNumberFormat("f");
    rectXZ->axis(QCPAxis::atBottom)->setNumberFormat("f");
    rectYZ->axis(QCPAxis::atBottom)->setNumberFormat("f");

    rectX->axis(QCPAxis::atLeft)->setNumberPrecision(ndp);
    rectY->axis(QCPAxis::atLeft)->setNumberPrecision(ndp);
    rectZ->axis(QCPAxis::atLeft)->setNumberPrecision(ndp);
    rectXY->axis(QCPAxis::atLeft)->setNumberPrecision(ndp);
    rectXZ->axis(QCPAxis::atLeft)->setNumberPrecision(ndp);
    rectYZ->axis(QCPAxis::atLeft)->setNumberPrecision(ndp);

    rectX->axis(QCPAxis::atBottom)->setNumberPrecision(ndp);
    rectY->axis(QCPAxis::atBottom)->setNumberPrecision(ndp);
    rectZ->axis(QCPAxis::atBottom)->setNumberPrecision(ndp);
    rectXY->axis(QCPAxis::atBottom)->setNumberPrecision(ndp);
    rectXZ->axis(QCPAxis::atBottom)->setNumberPrecision(ndp);
    rectYZ->axis(QCPAxis::atBottom)->setNumberPrecision(ndp);

    // Add them to the layout (Row, Column)
    customPlot->plotLayout()->addElement(0, 0, rectX);
    customPlot->plotLayout()->addElement(1, 0, rectY);
    customPlot->plotLayout()->addElement(2, 0, rectZ);
    customPlot->plotLayout()->addElement(0, 1, rectXY);
    customPlot->plotLayout()->addElement(1, 1, rectXZ);
    customPlot->plotLayout()->addElement(2, 1, rectYZ);

    // 2. Create the Graph Pointers
    // We link each graph to the axes of its specific rectangle
    QCPGraph *graphX = customPlot->addGraph(rectX->axis(QCPAxis::atBottom), rectX->axis(QCPAxis::atLeft));
    QCPGraph *graphY = customPlot->addGraph(rectY->axis(QCPAxis::atBottom), rectY->axis(QCPAxis::atLeft));
    QCPGraph *graphZ = customPlot->addGraph(rectZ->axis(QCPAxis::atBottom), rectZ->axis(QCPAxis::atLeft));
    QCPGraph *graphXY = customPlot->addGraph(rectXY->axis(QCPAxis::atBottom), rectXY->axis(QCPAxis::atLeft));
    QCPGraph *graphXZ = customPlot->addGraph(rectXZ->axis(QCPAxis::atBottom), rectXZ->axis(QCPAxis::atLeft));
    QCPGraph *graphYZ = customPlot->addGraph(rectYZ->axis(QCPAxis::atBottom), rectYZ->axis(QCPAxis::atLeft));

    graphXY->setLineStyle(QCPGraph::lsNone);
    graphXZ->setLineStyle(QCPGraph::lsNone);
    graphYZ->setLineStyle(QCPGraph::lsNone);
    graphXY->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    graphXZ->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    graphYZ->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));

    // 3. Assign Colors
    graphX->setPen(QPen(Qt::blue));
    graphY->setPen(QPen(Qt::red));
    graphZ->setPen(QPen(Qt::green));
    graphXY->setPen(QPen(Qt::blue));
    graphXZ->setPen(QPen(Qt::red));
    graphYZ->setPen(QPen(Qt::green));

    customPlot->plotLayout()->setRowSpacing(0);
    customPlot->plotLayout()->setMargins(QMargins(5, 5, 5, 5));

    rectX->setupFullAxesBox(true);
    rectY->setupFullAxesBox(true);
    rectZ->setupFullAxesBox(true);
    rectXY->setupFullAxesBox(true);
    rectXZ->setupFullAxesBox(true);
    rectYZ->setupFullAxesBox(true);

    startTime = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
    wsize = 0;
    QTimer *dataTimer = new QTimer(this);
    connect(dataTimer, &QTimer::timeout, this, [=]() {
        float x, y, z;
        double currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
        double diff_time = currentTime - this->startTime;

        this->accel->readXYZ(x, y, z);
        // printf("%.4f, %.4f, %.4f, %.4f\n", diff_time, x, y, z);

        this->vtime.push_back(diff_time);
        this->vx.push_back(x);
        this->vy.push_back(y);
        this->vz.push_back(z);

        // Keep only the last MAX_POINTS
        if (this->vx.size() > QT_SAMPLE_RATE) {
            this->vtime.erase(this->vtime.begin());
            this->vx.erase(this->vx.begin());
            this->vy.erase(this->vy.begin());
            this->vz.erase(this->vz.begin());
        }
        if (this->wsize > QT_SAMPLE_RATE) {
            this->startTime = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
            this->wsize = 0;
        }

        // Assuming you have time, xData, yData, and zData as QVector<double>
        graphX->setData(vtime, vx);
        graphY->setData(vtime, vy);
        graphZ->setData(vtime, vz);
        graphXY->setData(vx, vy);
        graphXZ->setData(vx, vz);
        graphYZ->setData(vy, vz);

        // graphX->rescaleValueAxis();
        // graphY->rescaleValueAxis();
        // graphZ->rescaleValueAxis();
        // graphXY->rescaleValueAxis();
        // graphXZ->rescaleValueAxis();
        // graphYZ->rescaleValueAxis();
        graphX->rescaleAxes();
        graphY->rescaleAxes();
        graphZ->rescaleAxes();
        graphXY->rescaleAxes();
        graphXZ->rescaleAxes();
        graphYZ->rescaleAxes();

        customPlot->replot();
        this->wsize++;
    });

    float sample_time = (1 / QT_SAMPLE_RATE) * 1000;
    dataTimer->start(sample_time);
}