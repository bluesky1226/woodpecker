
#include "reactant.h"
#include "coral.h"
#include <QLabel>
#include <QVBoxLayout>
#include <thread>
#include <unistd.h>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QCheckBox>


Reactant::Reactant(QWidget *parent) : QMainWindow(parent) {
    int ct = 0;
    for (const string &item : lnxdev_list) {
        ADXL345 *tmp = new ADXL345(item, -1);
        int rtc = -1;

        rtc = tmp->check_linux_device();
        if (rtc < 0) {
            adxl345_list[ct] = nullptr;
            printf("%s: ADXL345 linux device failed!!\n", item.c_str());
            ct++;
            continue;
        }

        rtc = tmp->check_adxl345_link();
        if (rtc < 0) {
            adxl345_list[ct] = nullptr;
            printf("%s: ADXL345 link failed!!\n", item.c_str());
            ct++;
            continue;
        }

        if (rtc >= 0) {
            printf("%s: ADXL345 init successfully\n", item.c_str());
            tmp->initialization();
            auto regs = tmp->readAllRegisters();
            for (size_t i = 0; i < regs.size(); i++) {
                printf("0x%02zX : 0x%02X\n", i, regs[i]);
            }
            adxl345_list[ct] = tmp;
        }
        ct++;
        sleep(2);
    }

    this->setWindowTitle("2x2 Window Toggler");
    this->resize(400, 300);

    QWidget *centralWidget = new QWidget(this);
    QGridLayout *gridLayout = new QGridLayout(centralWidget);
    setCentralWidget(centralWidget);

    for (int i = 0; i < 6; ++i) {
        int row = i / 3;
        int col = i % 3;

        QPushButton *btn = new QPushButton(QString("ADXL345_%1").arg(i + 1), this);
        btn->setCheckable(true);
        btn->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(250, 100, 100, 0.4);"
            "  color: rgba(0, 0, 0, 1);"
            "  border: 2px solid rgba(0, 0, 0, 1);"
            "  border-radius: 8px;"
            "  font-weight: bold;"
            "}"
        );

        if (adxl345_list[i] == nullptr) {
            btn->setEnabled(false);
            btn->setStyleSheet(
                "QPushButton {"
                "  background-color: rgba(250, 100, 100, 0.1);"
                "  color: rgba(0, 0, 0, 1);"
                "  border: 2px solid rgba(0, 0, 0, 1);"
                "  border-radius: 8px;"
                "  font-weight: bold;"
                "}"
            );
        }

        // Capture 'btn' in the lambda so we can modify it inside the function
        connect(btn, &QPushButton::toggled, this, [this, i, btn](bool checked) {
            handleToggle(checked, i, btn);
        });

        gridLayout->addWidget(btn, row, col);
    }
}

Reactant::~Reactant() {
    // Clean up any remaining windows
    for (auto win : m_subWindows) {
        win->deleteLater();
    }
}

void Reactant::handleToggle(bool checked, int index, QPushButton *btn) {
    if (checked) {
        // --- 1. Update Button Appearance ---
        btn->setText("ON");
        btn->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(0, 255, 0, 0.4);" 
            "  color: rgba(0, 0, 0, 1);"
            "  border: 2px solid rgba(0, 0, 0, 1);"
            "  border-radius: 8px;"
            "  font-weight: bold;"
            "}"
        );
        btn->setEnabled(false); // Lock it
        Coral *subWin = new Coral(nullptr, btn_name_list[index], adxl345_list[index]);
        subWin->show();

        connect(subWin, &QObject::destroyed, this, [btn, index, this]() {
            btn->setEnabled(true);
            btn->setChecked(false); // This resets the toggle state
            btn->setText(QString("ADXL345_%1").arg(index + 1));
            btn->setStyleSheet(
                "QPushButton {"
                "  background-color: rgba(250, 100, 100, 0.4);"
                "  color: rgba(0, 0, 0, 1);"
                "  border: 2px solid rgba(0, 0, 0, 1);"
                "  border-radius: 8px;"
                "  font-weight: bold;"
                "}"
            );
            // Clean up the map
            m_subWindows.remove(index);
        });

        m_subWindows[index] = subWin;
    }
}

void Reactant::set_plot_ind(QCustomPlot *customPlot, int index) {
    int ndp = 3;
    int rtb = 3;
    int rtc = 0;

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

        this->adxl345_list[index]->readXYZ(x, y, z);
        printf("%.4f, %.4f, %.4f, %.4f\n", diff_time, x, y, z);

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

        customPlot->replot();
        this->wsize++;
    });

    float sample_time = (1 / QT_SAMPLE_RATE) * 1000;
    dataTimer->start(sample_time);
}
