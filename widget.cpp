#include "widget.h"
#include "ui_widget.h"
#include "qmessagebox.h"
#include "qfiledialog.h"
#include <cmath>
#include <iostream>
#include "trackaxis.h"
#include <vector>
TrackAxis* image = nullptr;
vector<double> result;
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget){
    ui->setupUi(this);
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(test()));
    connect(ui->cacheButton_1, SIGNAL(clicked()), this, SLOT(cacheSet()));
    connect(ui->cacheButton_2, SIGNAL(clicked()), this, SLOT(cacheSet()));
    connect(ui->fileButton, SIGNAL(clicked()), this, SLOT(loadFile()));
    connect(ui->singleButton, SIGNAL(clicked()), this, SLOT(singleStep()));
    connect(ui->alongButton, SIGNAL(clicked()), this, SLOT(run()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(draw()));
    ui->lineEdit->setText("/Users/kangqi/Desktop/cc1.din");
    ui->cacheSize1->setCurrentIndex(5);
    ui->cacheSize2->setCurrentIndex(5);
    ui->cacheSize3->setCurrentIndex(5);
}

Widget::~Widget() {
    delete ui;
}

void Widget::loadFile(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open FaceClassifierFile"),
            "/", tr("in (*.*)"));
    ui->lineEdit->setText(fileName);
}

void Widget::cacheSet(){

   QRadioButton *clickedButton = qobject_cast<QRadioButton *>(sender());
   if(clickedButton != nullptr){
       if(clickedButton->objectName() == "cacheButton_1"){
           ui->cacheSize1->setEnabled(true);
           ui->cacheSize2->setEnabled(false);
           ui->cacheSize3->setEnabled(false);
       }else if(clickedButton->objectName() == "cacheButton_2"){
           ui->cacheSize1->setEnabled(false);
           ui->cacheSize2->setEnabled(true);
           ui->cacheSize3->setEnabled(true);
       }
   }
}
void Widget::singleStep(){
    if(sim == nullptr){
        initSimulation();
    }
    if(sim->mode == 0){
        sim->singleStep(index);
    }else{
        sim->singleStepMode2(index);
    }
    index++;
    result = sim->result;
    ui->sumLabel->setText(QString("访问总次数：") + QString::number((int)(result[0] + result[1]), 10));
    ui->missLabel->setText(QString("不命中次数：") + QString::number(result[1]));
    ui->missRate->setText(QString("不命中率：") + QString::number(result[1] * 100 / (result[0] + result[1]),'g',3) + "%");
    ui->insSumLabel->setText(QString("读指令次数：") + QString::number((int)(result[2] + result[3]), 10));
    ui->insMissLabel->setText(QString("不命中次数：") + QString::number(result[3]));
    ui->insMissRate->setText(QString("不命中率：") + QString::number(result[3] * 100 / (result[2] + result[3]),'g',3) + "%");
    ui->rdataSumLabel->setText(QString("读数据次数：") + QString::number((int)(result[4] + result[5]), 10));
    ui->rdataMissLabel->setText(QString("不命中次数：") + QString::number(result[5]));
    ui->rdataMissRate->setText(QString("不命中率：") + QString::number(result[5] * 100 / (result[4] + result[5]),'g',3) + "%");
    ui->wdataSumLabel->setText(QString("写数据次数：") + QString::number((int)(result[6] + result[7]), 10));
    ui->wdataMissLabel->setText(QString("不命中次数：") + QString::number(result[7]));
    ui->wdataMissRate->setText(QString("不命中率：") + QString::number(result[7] * 100 / (result[6] + result[7]),'g',3) + "%");
}
double Widget::run(){
    if(sim == nullptr){
        initSimulation();
    }
    sim->runAlong();
    //cout << result.size() << endl;
    result = sim->result;
    ui->sumLabel->setText(QString("访问总次数：") + QString::number((int)(result[0] + result[1]), 10));
    ui->missLabel->setText(QString("不命中次数：") + QString::number(result[1]));
    ui->missRate->setText(QString("不命中率：") + QString::number(result[1] * 100 / (result[0] + result[1]),'g',3) + "%");
    ui->insSumLabel->setText(QString("读指令次数：") + QString::number((int)(result[2] + result[3]), 10));
    ui->insMissLabel->setText(QString("不命中次数：") + QString::number(result[3]));
    ui->insMissRate->setText(QString("不命中率：") + QString::number(result[3] * 100 / (result[2] + result[3]),'g',3) + "%");
    ui->rdataSumLabel->setText(QString("读数据次数：") + QString::number((int)(result[4] + result[5]), 10));
    ui->rdataMissLabel->setText(QString("不命中次数：") + QString::number(result[5]));
    ui->rdataMissRate->setText(QString("不命中率：") + QString::number(result[5] * 100 / (result[4] + result[5]),'g',3) + "%");
    ui->wdataSumLabel->setText(QString("写数据次数：") + QString::number((int)(result[6] + result[7]), 10));
    ui->wdataMissLabel->setText(QString("不命中次数：") + QString::number(result[7]));
    ui->wdataMissRate->setText(QString("不命中率：") + QString::number(result[7] * 100 / (result[6] + result[7]),'g',3) + "%");
    delete sim;
    sim = nullptr;
    double tag = result[1] * 100 / (result[0] + result[1]);
    result.clear();
    return tag;
}
void Widget::test(){
    for(int i = 0; i < 10; i++){
        singleStep();
    }
}
void Widget::draw(){
    ui->progressBar->setValue(0);
    vector<vector<double>> drawData;
    if(sim == nullptr){
        sim = new cacheSimulation(ui->cacheButton_1->isChecked() ? 0 : 1, pow(2, ui->cacheSize1->currentIndex() + 1),
                              pow(2, ui->cacheSize2->currentIndex()), pow(2, ui->cacheSize3->currentIndex()),
                              pow(2, ui->blockSize->currentIndex() + 4), ui->xLD->currentIndex(), ui->tHCL->currentIndex(),
                              ui->yQCL->currentIndex(), ui->xCL->currentIndex(), ui->dKCL->currentIndex(), ui->lineEdit->text().toStdString());
    }
    if(ui->radioButton_3->isChecked()){
        ui->progressBar->setMaximum(30);
        if(ui->radioButton->isChecked()){
            for(int i = 0; i < 3; i++){
                vector<double> buf;
                for(int j = 0; j < 10; j++){
                        sim = new cacheSimulation(ui->cacheButton_1->isChecked() ? 0 : 1, pow(2, ui->cacheSize1->currentIndex() + 1),
                                              pow(2, ui->cacheSize2->currentIndex()), pow(2, ui->cacheSize3->currentIndex()),
                                              pow(2, ui->blockSize->currentIndex() + 4), ui->xLD->currentIndex(), ui->tHCL->currentIndex(),
                                              ui->yQCL->currentIndex(), ui->xCL->currentIndex(), ui->dKCL->currentIndex(), ui->lineEdit->text().toStdString());
                        sim->unifiedCacheSize = pow(2, j + 1);
                        sim->tHCL = i;
                        sim->xLD = 4;

                        QApplication::processEvents();
                        buf.push_back(100 * sim->runAlong());

                        delete sim;
                        sim = nullptr;
                        ui->progressBar->setValue(ui->progressBar->value() + 1);

                }
                drawData.push_back(buf);
                buf.clear();
            }
        }else if(ui->radioButton_2->isChecked()){
            ui->progressBar->setMaximum(60);
            for(int i = 0; i < 6; i++){
                vector<double> buf;
                for(int j = 0; j < 10; j++){
                        sim = new cacheSimulation(ui->cacheButton_1->isChecked() ? 0 : 1, pow(2, ui->cacheSize1->currentIndex() + 1),
                                              pow(2, ui->cacheSize2->currentIndex()), pow(2, ui->cacheSize3->currentIndex()),
                                              pow(2, ui->blockSize->currentIndex() + 4), ui->xLD->currentIndex(), ui->tHCL->currentIndex(),
                                              ui->yQCL->currentIndex(), ui->xCL->currentIndex(), ui->dKCL->currentIndex(), ui->lineEdit->text().toStdString());
                        sim->unifiedCacheSize = pow(2, j + 1);
                        sim->xLD = i;

                        QApplication::processEvents();
                        buf.push_back(100 * sim->runAlong());
                        delete sim;
                        sim = nullptr;
                        ui->progressBar->setValue(ui->progressBar->value() + 1);
                }
                drawData.push_back(buf);
                buf.clear();
            }
        }
    }else if(ui->radioButton_4->isChecked()){
        ui->progressBar->setMaximum(15);
        if(ui->radioButton->isChecked()){
            for(int i = 0; i < 3; i++){
                vector<double> buf;
                for(int j = 0; j < 5; j++){
                        sim = new cacheSimulation(ui->cacheButton_1->isChecked() ? 0 : 1, pow(2, ui->cacheSize1->currentIndex() + 1),
                                              pow(2, ui->cacheSize2->currentIndex()), pow(2, ui->cacheSize3->currentIndex()),
                                              pow(2, ui->blockSize->currentIndex() + 4), ui->xLD->currentIndex(), ui->tHCL->currentIndex(),
                                              ui->yQCL->currentIndex(), ui->xCL->currentIndex(), ui->dKCL->currentIndex(), ui->lineEdit->text().toStdString());
                        sim->blockSize = pow(2, j + 4);
                        sim->tHCL = i;
                        sim->xLD = 4;

                        QApplication::processEvents();
                        buf.push_back(100 * sim->runAlong());
                        delete sim;
                        sim = nullptr;

                        ui->progressBar->setValue(ui->progressBar->value() + 1);

                }
                drawData.push_back(buf);
                buf.clear();
            }
        }else if(ui->radioButton_2->isChecked()){
            ui->progressBar->setMaximum(30);
            for(int i = 0; i < 6; i++){
                vector<double> buf;
                for(int j = 0; j < 5; j++){
                        sim = new cacheSimulation(ui->cacheButton_1->isChecked() ? 0 : 1, pow(2, ui->cacheSize1->currentIndex() + 1),
                                              pow(2, ui->cacheSize2->currentIndex()), pow(2, ui->cacheSize3->currentIndex()),
                                              pow(2, ui->blockSize->currentIndex() + 4), ui->xLD->currentIndex(), ui->tHCL->currentIndex(),
                                              ui->yQCL->currentIndex(), ui->xCL->currentIndex(), ui->dKCL->currentIndex(), ui->lineEdit->text().toStdString());
                        sim->blockSize = pow(2, j + 4);
                        sim->xLD = i;

                        QApplication::processEvents();
                        buf.push_back(100 * sim->runAlong());
                        delete sim;
                        sim = nullptr;
                        ui->progressBar->setValue(ui->progressBar->value() + 1);
                }
                drawData.push_back(buf);
                buf.clear();
            }
        }
    }
    image = new TrackAxis(0, drawData);
    for(vector<double> & a : drawData){
        for(double b : a){
            cout << b << " ";
        }
        cout << endl;
    }
    image->show();
}

void Widget::initSimulation(){
    sim = new cacheSimulation(ui->cacheButton_1->isChecked() ? 0 : 1, pow(2, ui->cacheSize1->currentIndex() + 1),
                              pow(2, ui->cacheSize2->currentIndex()), pow(2, ui->cacheSize3->currentIndex()),
                              pow(2, ui->blockSize->currentIndex() + 4), ui->xLD->currentIndex(), ui->tHCL->currentIndex(),
                              ui->yQCL->currentIndex(), ui->xCL->currentIndex(), ui->dKCL->currentIndex(), ui->lineEdit->text().toStdString());
}

void Widget::reset(){
    ui->lineEdit->setText("");
    ui->blockSize->setCurrentIndex(1);
    ui->cacheSize1->setCurrentIndex(5);
    ui->cacheSize2->setCurrentIndex(5);
    ui->cacheSize3->setCurrentIndex(5);
    ui->xCL->setCurrentIndex(0);
    ui->dKCL->setCurrentIndex(0);
    ui->xLD->setCurrentIndex(0);
    ui->tHCL->setCurrentIndex(0);
    ui->yQCL->setCurrentIndex(0);
    ui->cacheButton_1->click();
    ui->addressFile->click();
    delete sim;
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Widget w;
    //TrackAxis *demo = new TrackAxis(w.ui->widget, d0, d1);
    w.show();
    //demo->show();
    return a.exec();
}
