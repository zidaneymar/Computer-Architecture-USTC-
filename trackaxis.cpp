#include <QApplication>
#include "trackaxis.h"
#include <sstream>
#include <algorithm>
using namespace std;


/*int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //app.setStyleSheet("* {font-family:arial;font-size:11px}");
    vector<double> d0 = {1,2,3,4,5};
    vector<double> d1 = {2,3,4,5,6};
    TrackAxis *demo = new TrackAxis(0, d0, d1);
    demo->show();
    return app.exec();
}
*/
TrackAxis::TrackAxis(QWidget *parent, vector<vector<double>>& data) :
    QDialog(parent)
{
    setWindowTitle("Track Line with Axis Labels");

    // Create the QChartViewer and draw the chart
    m_ChartViewer = new QChartViewer(this);
    drawChart(m_ChartViewer, data);

    // Set the window to be of the same size as the chart
    setFixedSize(m_ChartViewer->width(), m_ChartViewer->height());

    // Set up the mouseMovePlotArea handler for drawing the track cursor
    connect(m_ChartViewer, SIGNAL(mouseMovePlotArea(QMouseEvent*)),
        SLOT(onMouseMovePlotArea(QMouseEvent*)));
}

TrackAxis::~TrackAxis()
{
    delete m_ChartViewer->getChart();
}

//
// Draw the chart and display it in the given viewer
//
void TrackAxis::drawChart(QChartViewer *viewer,vector<vector<double>>& data)
{
    // Data for the chart as 2 random data series


    XYChart *c = new XYChart(670, 400);

    // Add a title to the chart using 18 pts Times New Roman Bold Italic font
    c->addTitle("DRAWING RESULT", "timesbi.ttf", 18);

    // Set the plotarea at (50, 55) with width 100 pixels less than chart width, and height 90 pixels
    // less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
    // as background. Set border to transparent and grid lines to white (ffffff).
    c->setPlotArea(50, 55, c->getWidth() - 100, c->getHeight() - 90, c->linearGradientColor(0, 55, 0,
        c->getHeight() - 35, 0xf0f6ff, 0xa0c0ff), -1, Chart::Transparent, 0xffffff, 0xffffff);

    // Add a legend box at (50, 25) using horizontal layout. Use 10pts Arial Bold as font. Set the
    // background and border color to Transparent.
    c->addLegend(50, 25, false, "arialbd.ttf", 10)->setBackground(Chart::Transparent);

    // Set axis label style to 8pts Arial Bold
    c->xAxis()->setLabelStyle("arialbd.ttf", 8);
    c->yAxis()->setLabelStyle("arialbd.ttf", 8);
    //c->yAxis2()->setLabelStyle("arialbd.ttf", 8);

    // Set the axis stem to transparent
    c->xAxis()->setColors(Chart::Transparent);
    c->yAxis()->setColors(Chart::Transparent);
    //c->yAxis2()->setColors(Chart::Transparent);

    // Configure x-axis label format
    //c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(), "{value|mm/yyyy} ",
       // Chart::StartOfMonthFilter(), "{value|mm}");

    //c->xAxis()->setMultiFormat(Chart::NonePassFilter(),"", 1, true);
    // Add axis title using 10pts Arial Bold Italic font
    c->yAxis()->setTitle("Missing Rate(%)", "arialbi.ttf", 10);
    //c->yAxis2()->setTitle("Effective Load (kg)", "arialbi.ttf", 10);

    // Add a line layer to the chart using a line width of 2 pixels.
    LineLayer *layer = c->addLineLayer();
    layer->setLineWidth(2);

    // Add 2 data series to the line layer
    //layer->setXData(timeStamps);
    if(data.size() == 3){

        layer->addDataSet(DoubleArray(data[0].data(), data[0].size()), 0xcc0000, "LRU");
        layer->addDataSet(DoubleArray(data[1].data(), data[1].size()), 0x008800, "FIFO");
        layer->addDataSet(DoubleArray(data[2].data(), data[2].size()), 0x000099, "RAND");
    }else if(data.size() == 6){
        layer->addDataSet(DoubleArray(data[0].data(), data[0].size()), 0xcc0000, "straight-connect");
        layer->addDataSet(DoubleArray(data[1].data(), data[1].size()), 0x008800, "2-way");
        layer->addDataSet(DoubleArray(data[2].data(), data[2].size()), 0x000099, "4-way");
        layer->addDataSet(DoubleArray(data[3].data(), data[3].size()), 0xcc8800, "8-way");
        layer->addDataSet(DoubleArray(data[4].data(), data[4].size()), 0x008899, "16-way");
        layer->addDataSet(DoubleArray(data[5].data(), data[5].size()), 0xcc0099, "32-way");
    }
    viewer->setChart(c);
}

//
// Draw track cursor when mouse is moving over plotarea
//
void TrackAxis::onMouseMovePlotArea(QMouseEvent *)
{
    trackLineAxis((XYChart *)m_ChartViewer->getChart(), m_ChartViewer->getPlotAreaMouseX());
    m_ChartViewer->updateDisplay();
    
    // Hide the track cursor when the mouse leaves the plot area
    m_ChartViewer->removeDynamicLayer("mouseLeavePlotArea");
}

//
// Draw track line with axis labels
//
void TrackAxis::trackLineAxis(XYChart *c, int mouseX)
{
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea *d = c->initDynamicLayer();

    // The plot area object
    PlotArea *plotArea = c->getPlotArea();

    // Get the data x-value that is nearest to the mouse, and find its pixel coordinate.
    double xValue = c->getNearestXValue(mouseX);
    int xCoor = c->getXCoor(xValue);

    // The vertical track line is drawn up to the highest data point (the point with smallest
    // y-coordinate). We need to iterate all datasets in all layers to determine where it is.
    int minY = plotArea->getBottomY();

    // Iterate through all layers to find the highest data point
    for(int i = 0; i < c->getLayerCount(); ++i) {
        Layer *layer = c->getLayerByZ(i);

        // The data array index of the x-value
        int xIndex = layer->getXIndexOf(xValue);

        // Iterate through all the data sets in the layer
        for(int j = 0; j < layer->getDataSetCount(); ++j) {
            DataSet *dataSet = layer->getDataSetByZ(j);

            double dataPoint = dataSet->getPosition(xIndex);
            if ((dataPoint != Chart::NoValue) && (dataSet->getDataColor() != (int)Chart::Transparent))
                minY = min(minY, c->getYCoor(dataPoint, dataSet->getUseYAxis()));
        }
    }

    // Draw a vertical track line at the x-position up to the highest data point.
    d->vline(max(minY, plotArea->getTopY()), plotArea->getBottomY() + 6, xCoor, d->dashLineColor(
        0x000000, 0x0101));

    // Draw a label on the x-axis to show the track line position
    ostringstream xlabel;
    xlabel << "<*font,bgColor=000000*> " << c->xAxis()->getFormattedLabel(xValue, "mmm dd, yyyy")
        << " <*/font*>";
    TTFText *t = d->text(xlabel.str().c_str(), "arialbd.ttf", 8);
    t->draw(xCoor, plotArea->getBottomY() + 6, 0xffffff, Chart::Top);
    t->destroy();

    // Iterate through all layers to build the legend array
    for(int i = 0; i < c->getLayerCount(); ++i) {
        Layer *layer = c->getLayerByZ(i);

        // The data array index of the x-value
        int xIndex = layer->getXIndexOf(xValue);

        // Iterate through all the data sets in the layer
        for(int j = 0; j < layer->getDataSetCount(); ++j) {
            DataSet *dataSet = layer->getDataSetByZ(j);

            // The positional value, axis binding, pixel coordinate and color of the data point.
            double dataPoint = dataSet->getPosition(xIndex);
            Axis *yAxis = dataSet->getUseYAxis();
            int yCoor = c->getYCoor(dataPoint, yAxis);
            int color = dataSet->getDataColor();

            // Draw the axis label only for visible data points of named data sets
            if ((dataPoint != Chart::NoValue) && (color != (int)Chart::Transparent) && (yCoor >=
                plotArea->getTopY()) && (yCoor <= plotArea->getBottomY())) {
                // The axis label consists of 3 parts - a track dot for the data point, an axis label,
                // and a line joining the track dot to the axis label.

                // Draw the line first. The end point of the line at the axis label side depends on
                // whether the label is at the left or right side of the axis (that is, on whether the
                // axis is on the left or right side of the plot area).
                int xPos = yAxis->getX() + ((yAxis->getAlignment() == Chart::Left) ? -4 : 4);
                d->hline(xCoor, xPos, yCoor, d->dashLineColor(color, 0x0101));

                // Draw the track dot
                d->circle(xCoor, yCoor, 4, 4, color, color);

                // Draw the axis label. If the axis is on the left side of the plot area, the labels
                // should right aligned to the axis, and vice versa.
                ostringstream axisLabel;
                axisLabel << "<*font,bgColor=" << hex << color << "*> "
                    << c->formatValue(dataPoint, "{value|P4}")	<< " <*/font*>";
                t = d->text(axisLabel.str().c_str(), "arialbd.ttf", 8);
                t->draw(xPos, yCoor, 0xffffff,
                    ((yAxis->getAlignment() == Chart::Left) ? Chart::Right : Chart::Left));
                t->destroy();
            }
        }
    }
}
