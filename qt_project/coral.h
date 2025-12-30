#ifndef CORAL_H
#define CORAL_H

#include "qcustomplot.h"
#include "adxl345.h"
#include <QMap>
#include <QMainWindow>
#include <vector>
#include <map>

#define QT_SAMPLE_RATE  1500    // Hz

class Coral: public QWidget {
    Q_OBJECT

public:
    double startTime;
    int wsize;
    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;
    QPushButton *btn_calibration;
    QPushButton *btn_save;
    QLineEdit *le_savetime;
    QCheckBox *cb_xe;
    QPushButton *btn_run;
    QCPAxisRect *rectX = nullptr;
    QCPAxisRect *rectY = nullptr;
    QCPAxisRect *rectZ = nullptr;
    QCPAxisRect *rectXY = nullptr;
    QCPAxisRect *rectXZ = nullptr;
    QCPAxisRect *rectYZ = nullptr;
    QCPGraph *graphX;
    QCPGraph *graphY;
    QCPGraph *graphZ;
    QCPGraph *graphXY;
    QCPGraph *graphXZ;
    QCPGraph *graphYZ;
    QCustomPlot *coralplot;

    Coral(QWidget *parent = nullptr, QString qs = QString(), ADXL345 *device = nullptr);
    ~Coral();

    void run_btn_tgl(bool check, QPushButton *btn);
    void cal_btn_tgl(bool check)

private:
    QVector<double> vtime, vx, vy, vz;
    ADXL345 *accl;

};

#endif