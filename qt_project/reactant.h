
#ifndef REACTANT_H
#define REACTANT_H

#include "adxl345.h"
#include "qcustomplot.h"
#include "coral.h"
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QMap>
#include <vector>
#include <map>

#define ADXL345_NUM     8
#define MAX_DATA        300
#define QT_SAMPLE_RATE  1500    // Hz

class Reactant: public QMainWindow {
    Q_OBJECT

public:
    double startTime;
    int wsize;
    vector<QString> btn_name_list = {
        "ADXL345_1",
        "ADXL345_2",
        "ADXL345_3",
        "ADXL345_4",
        "ADXL345_5",
        "ADXL345_6",
        "ADXL345_7",
        "ADXL345_8"
    };
    vector<string> lnxdev_list = {
        "/dev/spidev0.0",
        "/dev/spidev0.1",
        "/dev/spidev1.0",
        "/dev/spidev1.1",
        "/dev/spidev1.2"
    };

    map<string, int> cs_dic = {
        {"/dev/spidev0.0", 5},
        {"/dev/spidev0.0", 6},
        {"/dev/spidev1.0", 26}
    };
    vector<ADXL345*> adxl345_list = {
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr
    };

    Reactant(QWidget *parent = nullptr);
    ~Reactant();

private slots:
    void handleToggle(bool checked, int index, QPushButton *btn);
    void set_plot_ind(QCustomPlot *customPlot, int index);

private:
    QVector<double> vtime, vx, vy, vz;
    QMap<int, Coral*> m_subWindows; // Stores windows by index
};

#endif