
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 1. Create the main window
    QWidget *window = new QWidget();
    window->setWindowTitle("Qt 2x2 Grid");
    window->resize(300, 200);

    // 2. Create a Grid Layout
    QGridLayout *layout = new QGridLayout(window);

    // 3. Create and add 4 buttons to the grid
    // addWidget syntax: (widget, row, column)
    QPushButton *btn1 = new QPushButton("Button (0,0)");
    QPushButton *btn2 = new QPushButton("Button (0,1)");
    QPushButton *btn3 = new QPushButton("Button (1,0)");
    QPushButton *btn4 = new QPushButton("Button (1,1)");

    layout->addWidget(btn1, 0, 0);
    layout->addWidget(btn2, 0, 1);
    layout->addWidget(btn3, 1, 0);
    layout->addWidget(btn4, 1, 1);

    // 4. Simple Click Action (Example)
    QObject::connect(btn1, &QPushButton::clicked, []() {
        qDebug() << "Top-left button clicked!";
    });

    QObject::connect(btn2, &QPushButton::clicked, []() {
        qDebug() << "Top-right button clicked!";
    });

    QObject::connect(btn3, &QPushButton::clicked, []() {
        qDebug() << "Button-left button clicked!";
    });

    QObject::connect(btn4, &QPushButton::clicked, []() {
        qDebug() << "Butoon-right button clicked!";
    });

    window->show();
    return app.exec();
}