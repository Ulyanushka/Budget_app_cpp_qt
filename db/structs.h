#ifndef DB_STRUCTS_H
#define DB_STRUCTS_H

#include <QDate>
#include <QVector>


struct Period {
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
    Period period;
    QVector<Income> incomes;
    QVector<Expense> expenses;
};

#endif // DB_STRUCTS_H
