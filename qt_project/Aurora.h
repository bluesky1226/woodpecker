#ifndef AURORA_H
#define AURORA_H

#include <QMainWindow>
#include "qcustomplot.h" // Include the plotting library
#include "adxl345.h"
#include <QVector>

using namespace std;

class Aurora : public QMainWindow {
    Q_OBJECT

public:
    double startTime;
    int wsize;

    Aurora(QWidget *parent = nullptr);
    ~Aurora();

private:
    ADXL345 *accel = nullptr;
    QVector<double> vtime, vx, vy, vz;
};

#endif // AURORA_H