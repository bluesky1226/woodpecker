#ifndef CORAL_H
#define CORAL_H

#include "qcustomplot.h"
#include "adxl345.h"
#include <QMap>
#include <QMainWindow>
#include <vector>
#include <map>

// #define QT_SAMPLE_RATE  10    // Hz
#define QT_SAMPLE_RATE  1000    // default Hz

class Coral: public QWidget {
    Q_OBJECT

public:
    double startTime;
    int wsize;
    int chart_index;
    QTimer *dataTimer;
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

    Coral(QWidget *parent = nullptr, QString qs = QString(), ADXL345 *device = nullptr, int index = 0);
    ~Coral();

    void set_plot_limit(const QVector<double> vdata, int *bmin, int *bmax);

private slots:
    void run_btn_tgl(bool checked, QPushButton *btn);
    void cal_btn_tgl();
    int save_btn_tgl();

private:
    QVector<double> vtime, vx, vy, vz;
    ADXL345 *accl;

};

#endif