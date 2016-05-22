#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "cachesimulation.h"
#include "vector"
namespace Ui {
class Widget;
}

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    Ui::Widget *ui;
//public slots:

private slots:
    void reset();
    void cacheSet();
    void loadFile();
    void singleStep();
    double run();
    void test();
    void draw();
private:
    cacheSimulation *sim = nullptr;
    void initSimulation();
    int index = 0;
};

#endif // WIDGET_H
