#ifndef TESTS_MAINWINDOW_H
#define TESTS_MAINWINDOW_H

#include <QObject>


class TestMainWindow : public QObject
{
    Q_OBJECT

private slots:
    void addPositiveNumbers();
    void addNegativeNumbers();
    void addZero();
    void greetingTest();
    void greetingEmptyName();
};

#endif // TESTS_MAINWINDOW_H
