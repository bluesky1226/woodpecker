
#include "coral.h"
#include "adxl345.h"
#include <QMainWindow>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <QLineEdit>
#include <thread>
#include <QDebug>
// #include <QtConcurrent>

using namespace std;

Coral::Coral(QWidget *parent, QString qs, ADXL345 *device, int index) : QWidget(parent) {
    int ndp = 2;
    int rtb = 3;
    int rtc = 0;

    accl = device;
    chart_index = index + 1;
    this->setWindowTitle(qs);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->resize(1200, 900); // Made it wider to fit all elements in a row
    coralplot = new QCustomPlot(this);

    vLayout = new QVBoxLayout(this);

    hLayout = new QHBoxLayout();
    btn_calibration = new QPushButton("Calibration");
    btn_save = new QPushButton("Save");
    le_savetime = new QLineEdit();
    le_savetime->setPlaceholderText("Type here...");
    cb_xe = new QCheckBox("Enable");
    btn_run = new QPushButton("Run");

    hLayout->addWidget(btn_calibration);
    hLayout->addWidget(btn_save);
    hLayout->addWidget(le_savetime, 1);
    hLayout->addWidget(cb_xe);
    hLayout->addWidget(btn_run);
    hLayout->addStretch();

    vLayout->addLayout(hLayout);
    vLayout->addWidget(coralplot);

    // 1. Prepare the layout
    coralplot->plotLayout()->clear();

    // Create 3 Rects (Charts)
    rectX = new QCPAxisRect(coralplot);
    rectY = new QCPAxisRect(coralplot);
    rectZ = new QCPAxisRect(coralplot);
    rectXY = new QCPAxisRect(coralplot);
    rectXZ = new QCPAxisRect(coralplot);
    rectYZ = new QCPAxisRect(coralplot);

    rectX->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectY->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectZ->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectXY->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectXZ->axis(QCPAxis::atLeft)->setNumberFormat("f");
    rectYZ->axis(QCPAxis::atLeft)->setNumberFormat("f");

    rectX->axis(QCPAxis::atBottom)->setNumberFormat("gb");
    rectY->axis(QCPAxis::atBottom)->setNumberFormat("gb");
    rectZ->axis(QCPAxis::atBottom)->setNumberFormat("gb");
    rectXY->axis(QCPAxis::atBottom)->setNumberFormat("f");
    rectXZ->axis(QCPAxis::atBottom)->setNumberFormat("f");
    rectYZ->axis(QCPAxis::atBottom)->setNumberFormat("f");

    rectX->axis(QCPAxis::atLeft)->setNumberPrecision(1);
    rectY->axis(QCPAxis::atLeft)->setNumberPrecision(1);
    rectZ->axis(QCPAxis::atLeft)->setNumberPrecision(1);
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
    coralplot->plotLayout()->addElement(0, 0, rectX);
    coralplot->plotLayout()->addElement(1, 0, rectY);
    coralplot->plotLayout()->addElement(2, 0, rectZ);
    coralplot->plotLayout()->addElement(0, 1, rectXY);
    coralplot->plotLayout()->addElement(1, 1, rectXZ);
    coralplot->plotLayout()->addElement(2, 1, rectYZ);

    // 2. Create the Graph Pointers
    // We link each graph to the axes of its specific rectangle
    graphX = coralplot->addGraph(rectX->axis(QCPAxis::atBottom), rectX->axis(QCPAxis::atLeft));
    graphY = coralplot->addGraph(rectY->axis(QCPAxis::atBottom), rectY->axis(QCPAxis::atLeft));
    graphZ = coralplot->addGraph(rectZ->axis(QCPAxis::atBottom), rectZ->axis(QCPAxis::atLeft));
    graphXY = coralplot->addGraph(rectXY->axis(QCPAxis::atBottom), rectXY->axis(QCPAxis::atLeft));
    graphXZ = coralplot->addGraph(rectXZ->axis(QCPAxis::atBottom), rectXZ->axis(QCPAxis::atLeft));
    graphYZ = coralplot->addGraph(rectYZ->axis(QCPAxis::atBottom), rectYZ->axis(QCPAxis::atLeft));

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

    coralplot->plotLayout()->setRowSpacing(0);
    coralplot->plotLayout()->setMargins(QMargins(5, 5, 5, 5));

    rectX->setupFullAxesBox(true);
    rectY->setupFullAxesBox(true);
    rectZ->setupFullAxesBox(true);
    rectXY->setupFullAxesBox(true);
    rectXZ->setupFullAxesBox(true);
    rectYZ->setupFullAxesBox(true);

    vLayout->addLayout(hLayout);      // Controls at top
    vLayout->addWidget(this, 1); // Plot at bottom (1 = expands)

    connect(btn_save, &QPushButton::clicked, this, [this]() {
        this->btn_save->setText("processing");
        this->btn_save->setEnabled(false);
        this->btn_run->setEnabled(false);
        this->btn_calibration->setEnabled(false);
        this->btn_save->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        this->btn_save->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(255, 0, 0, 1);"
            "  color: rgba(0, 0, 0, 1);"
            "  border: 2px solid rgba(0, 0, 0, 1);"
            "  border-radius: 8px;"
            "  font-weight: bold;"
            "}"
        );
        std::thread worker([this]() {
            this->save_btn_tgl();

            QMetaObject::invokeMethod(this, [this]() {
                this->btn_save->setText("Save");
                this->btn_save->setEnabled(true);
                this->btn_run->setEnabled(true);
                this->btn_calibration->setEnabled(true);
                this->btn_save->setStyleSheet("");
            });
        });
        worker.detach();
    });

    connect(btn_calibration, &QPushButton::clicked, this, [this]() {
        this->btn_calibration->setText("processing");
        this->btn_calibration->setEnabled(false);
        this->btn_run->setEnabled(false);
        this->btn_save->setEnabled(false);
        this->btn_calibration->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        this->btn_calibration->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(255, 0, 0, 1);"
            "  color: rgba(0, 0, 0, 1);"
            "  border: 2px solid rgba(0, 0, 0, 1);"
            "  border-radius: 8px;"
            "  font-weight: bold;"
            "}"
        );
        std::thread worker([this]() {
            this->cal_btn_tgl();

            QMetaObject::invokeMethod(this, [this]() {
                this->btn_calibration->setText("Calibration");
                this->btn_calibration->setEnabled(true);
                this->btn_run->setEnabled(true);
                this->btn_save->setEnabled(true);
                this->btn_calibration->setStyleSheet("");
            });
        });
        worker.detach();
    });

    btn_run->setCheckable(true);
    connect(btn_run, &QPushButton::toggled, this, [this](bool checked) {
        run_btn_tgl(checked, btn_run);
    });

    this->startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    wsize = 0;
    dataTimer = new QTimer(this);
    vx.clear();
    vy.clear();
    vz.clear();
    connect(dataTimer, &QTimer::timeout, this, [=]() {
        float x, y, z;
        int bmin_x, bmax_x, bmin_y, bmax_y, bmin_z, bmax_z, bmin_t, bmax_t;
        qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qint64 diff_time = currentTime - this->startTime;
        qint64 run_timer_diff = currentTime - this->run_timer_start;
        qint64 run_timer_diff_min = run_timer_diff / 1000 / 60;

        if (run_timer_diff_min >= RUN_TIMER_LIMIT)
        {
            this->dataTimer->stop();
            this->btn_run->setText("Run");
            this->btn_run->setChecked(false);
            this->startTime = 0;
            this->run_timer_start = 0;
            this->vx.clear();
            this->vy.clear();
            this->vz.clear();
            this->vtime.clear();
            this->dataTimer->stop();
            this->btn_calibration->setEnabled(true);
            this->btn_save->setEnabled(true);
        }

        accl->readXYZ(x, y, z);
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
            this->startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
            this->wsize = 0;
        }

        auto result = minmax_element(vtime.begin(), vtime.end());
        bmax_t = *result.second;

        set_plot_limit(this->vx, &bmin_x, &bmax_x);
        set_plot_limit(this->vy, &bmin_y, &bmax_y);
        set_plot_limit(this->vz, &bmin_z, &bmax_z);

        graphX->keyAxis()->setRange(0, bmax_t);
        graphX->valueAxis()->setRange(bmin_x, bmax_x);

        graphY->keyAxis()->setRange(0, bmax_t);
        graphY->valueAxis()->setRange(bmin_y, bmax_y);

        graphZ->keyAxis()->setRange(0, bmax_t);
        graphZ->valueAxis()->setRange(bmin_z, bmax_z);

        graphXY->keyAxis()->setRange(bmin_x, bmax_x);
        graphXY->valueAxis()->setRange(bmin_y, bmax_y);

        graphXZ->keyAxis()->setRange(bmin_x, bmax_x);
        graphXZ->valueAxis()->setRange(bmin_z, bmax_z);

        graphYZ->keyAxis()->setRange(bmin_y, bmax_y);
        graphYZ->valueAxis()->setRange(bmin_z, bmax_z);

        graphX->setData(vtime, vx);
        graphY->setData(vtime, vy);
        graphZ->setData(vtime, vz);
        graphXY->setData(vx, vy);
        graphXZ->setData(vx, vz);
        graphYZ->setData(vy, vz);

        coralplot->replot();
        this->wsize++;
    });
}

Coral::~Coral()
{
}

void Coral::run_btn_tgl(bool checked, QPushButton *btn)
{
    if (checked) {
        btn->setText("Stop");
        this->startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        this->run_timer_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
        float sample_time = (1 / QT_SAMPLE_RATE) * 1000;
        this->dataTimer->start(sample_time);
        this->btn_calibration->setEnabled(false);
        this->btn_save->setEnabled(false);
    } else {
        btn->setText("Run");
        this->startTime = 0;
        this->run_timer_start = 0;
        this->vx.clear();
        this->vy.clear();
        this->vz.clear();
        this->vtime.clear();
        this->dataTimer->stop();
        this->btn_calibration->setEnabled(true);
        this->btn_save->setEnabled(true);
    }
}

void Coral::cal_btn_tgl()
{
    int repeat = 20;
    printf("Starting to do the calibation ...\n");
    sleep(1);
    this->accl->offsetreg_value();
    float x, y, z;
    float x_t, y_t, z_t;
    x_t = 0;
    y_t = 0;
    z_t = 0;
    sleep(1);
    for (int i = 0; i < repeat; i++)
    {
        this->accl->readXYZ(x, y, z);
        x_t += x;
        y_t += y;
        z_t += z;
        printf("X:%.4f, Y:%.4f, Z:%.4f, X_TOL:%.4f, Y_TOL:%.4f, Z_TOL:%.4f\n", x, y, z, x_t, y_t, z_t);
        sleep(1);
    }
    float x_avg, y_avg, z_avg;
    x_avg = x_t / repeat;
    y_avg = y_t / repeat;
    z_avg = z_t / repeat;
    printf("X axis average : %.4f\n", x_avg);
    printf("Y axis average : %.4f\n", y_avg);
    printf("Z axis average : %.4f\n", z_avg);
    this->accl->adxl345_write_offsets(x_avg, y_avg, z_avg);
    printf("ADXL345 Calibration is done\n");
}

int Coral::save_btn_tgl()
{
    string accl_index;
    int sv_period = 20;
    float x, y, z;

    try {
        auto t = time(nullptr);
        auto tm = *localtime(&t);
        accl_index = to_string(this->chart_index);

        ostringstream filename;
        filename << "adxl345_"
                 << accl_index
                 << "_"
                 << put_time(&tm, "%Y%m%d_%H%M%S")
                 << ".csv";

        ofstream file(filename.str());
        if (!file.is_open()) {
            cerr << "Failed to open CSV file\n";
            return -1;
        }

        // ===== Start time =====
        auto start = chrono::steady_clock::now();
        // ======================

        while (true) {
            this->accl->readXYZ(x, y, z);

            auto now = chrono::steady_clock::now();
            auto ts = chrono::duration_cast<chrono::microseconds>(
                          now.time_since_epoch())
                          .count();

            file << x << "," << y << "," << z << "\n";
            file.flush();

            auto elapsed = chrono::duration_cast<chrono::seconds>(now - start).count();
            if (elapsed >= sv_period) {
                break;
            }

            usleep(QT_SAMPLE_RATE);
        }

        printf("Data collection finished (%d seconds).\n", sv_period);
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}

void Coral::set_plot_limit(const QVector<double> vdata, int *bmin, int *bmax)
{
    float v_min, v_max, v_avg, v_sum;

    auto result = minmax_element(vdata.begin(), vdata.end());
    v_min = *result.first;
    v_max = *result.second;
    v_sum = accumulate(vdata.begin(), vdata.end(), 0.0);
    v_avg = v_sum / vdata.size();

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

    if (v_min >= 0)
    {
        *bmin = ceil(v_min);
    }
    else
    {
        *bmin = floor(v_min);
    }

    if (v_max >= 0)
    {
        *bmax = ceil(v_max);
    }
    else
    {
        *bmax = floor(v_max);
    }
}
