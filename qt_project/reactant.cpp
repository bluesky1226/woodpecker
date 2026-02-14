
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
    initialize_adx345();

    this->setWindowTitle("3x2 Window Toggler");
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

void Reactant::initialize_adx345() {
    int ct = 0;
    for (const string &item : lnx_dev_name_list) {
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
        Coral *subWin = new Coral(nullptr, btn_name_list[index], adxl345_list[index], index);
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

