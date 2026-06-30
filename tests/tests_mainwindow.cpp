#include "tests_mainwindow.h"

#include <QtTest>
#include "../mainwindow.h"


void TestMainWindow::addPositiveNumbers()
{
    MainWindow w;
    QCOMPARE(w.add(2, 3), 5);
}

void TestMainWindow::addNegativeNumbers()
{
    MainWindow w;
    QCOMPARE(w.add(-1, -2), -3);
}

void TestMainWindow::addZero()
{
    MainWindow w;
    QCOMPARE(w.add(10, 0), 10);
}

void TestMainWindow::greetingTest()
{
    MainWindow w;
    QCOMPARE(w.greeting("Alice"), QString("Hello, Alice!"));
}

void TestMainWindow::greetingEmptyName()
{
    MainWindow w;
    QCOMPARE(w.greeting(""), QString("Hello, !"));
}
