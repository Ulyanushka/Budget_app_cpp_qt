#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::add(int a, int b) const
{
    return a + b;
}

QString MainWindow::greeting(const QString &name) const
{
    return QString("Hello, %1!").arg(name);
}
