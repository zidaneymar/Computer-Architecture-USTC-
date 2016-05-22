#ifndef TRACKAXIS_H
#define TRACKAXIS_H

#include <QDialog>
#include "qchartviewer.h"
using namespace std;

class TrackAxis : public QDialog {
    Q_OBJECT
public:
    TrackAxis(QWidget *parent, vector<vector<double>>&);
    ~TrackAxis();

private:
    QChartViewer *m_ChartViewer;

    void drawChart(QChartViewer *viewer, vector<vector<double>>&);           // Draw chart
    void trackLineAxis(XYChart *c, int mouseX);     // Draw Track Cursor

private slots:
    void onMouseMovePlotArea(QMouseEvent *event);
};

#endif // TRACKAXIS_H
