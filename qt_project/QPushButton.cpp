
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <vector>
#include <string>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget *window = new QWidget();
    window->setWindowTitle("Sensor Control Panel");
    window->resize(400, 300);

    QGridLayout *layout = new QGridLayout(window);

    // Labels for our 4 buttons
    std::vector<std::string> labels = {"X-Axis", "Y-Axis", "Z-Axis", "Log Data"};

    for (int i = 0; i < 4; ++i) {
        // Calculate row and column for 2x2 grid
        int row = i / 2;
        int col = i % 2;

        QPushButton *btn = new QPushButton(QString::fromStdString(labels[i] + ": OFF"));
        
        // --- The Toggle Magic ---
        btn->setCheckable(true); 
        btn->setMinimumHeight(60); // Make them easier to click on a small screen

        // Lambda function to handle the logic when pressed
        QObject::connect(btn, &QPushButton::toggled, [btn, labels, i](bool checked) {
            if (checked) {
                btn->setText(QString::fromStdString(labels[i] + ": ON"));
                btn->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
            } else {
                btn->setText(QString::fromStdString(labels[i] + ": OFF"));
                btn->setStyleSheet(""); // Reset to default system style
            }
        });

        layout->addWidget(btn, row, col);
    }

    window->show();
    return app.exec();
}