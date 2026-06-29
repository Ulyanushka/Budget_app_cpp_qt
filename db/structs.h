#ifndef DB_STRUCTS_H
#define DB_STRUCTS_H

#include <QString>
#include <QDate>
#include <QVector>


struct BudgetPeriod {
    QDate start;
    QDate end;
};

struct Income {
    QString source;
    int amount;
};

struct Expense {
    QString article;
    QVector<int> amounts;
    QVector<QString> comments;
};

struct BudgetData {
    QString name;
    BudgetPeriod period;
    QVector<Income> incomes;
    QVector<Expense> expenses;
};

#endif // DB_STRUCTS_H
