#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGraphicsScene *scene =  new QGraphicsScene(0,0,640,480);

    scene->setBackgroundBrush(QColor(50,50,50));
    ui->worldView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}
