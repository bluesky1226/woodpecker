
#include "coral.h"
#include "adxl345.h"
#include <QMainWindow>
#include <QLineEdit>

Coral::Coral(QWidget *parent, QString qs, ADXL345 *device) : QWidget(parent) {
    int ndp = 3;
    int rtb = 3;
    int rtc = 0;

    accl = device;
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

    connect(btn_calibration, &QPushButton::toggled, this, [this, btn_run](bool checked) {
        cal_btn_tgl(checked, btn_run);
    });

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

    startTime = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
    wsize = 0;
    QTimer *dataTimer = new QTimer(this);
    connect(dataTimer, &QTimer::timeout, this, [=]() {
        float x, y, z;
        double currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
        double diff_time = currentTime - this->startTime;

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

        graphX->rescaleAxes();
        graphY->rescaleAxes();
        graphZ->rescaleAxes();
        graphXY->rescaleAxes();
        graphXZ->rescaleAxes();
        graphYZ->rescaleAxes();

        coralplot->replot();
        this->wsize++;
    });

    float sample_time = (1 / QT_SAMPLE_RATE) * 1000;
    dataTimer->start(sample_time);
}

Coral::~Coral()
{
}

void Coral::run_btn_tgl(bool check, QPushButton *btn)
{
}

void Coral::cal_btn_tgl(bool check, ADXL345 *device)
{
}
