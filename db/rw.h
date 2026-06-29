#ifndef DB_RW_H
#define DB_RW_H

#include <QVector>
#include <QJsonObject>

#include "structs.h"



void WritePeriodToJson(QJsonObject& json, const Period& period);
void WriteIncomesToJson(QJsonObject& json, const QVector<Income>& incomes);
void WriteExpensesToJson(QJsonObject& json, const QVector<Expense>& expenses);

bool ReadPeriodFromJson(const QJsonObject& json, Period& period);
bool ReadIncomesFromJson(const QJsonObject& json, QVector<Income>& incomes);
bool ReadExpensesFromJson(const QJsonObject& json, QVector<Expense>& expenses);

bool WriteBudgetDataToJson(const BudgetData& data, const QString& dir_path);
bool ReadBudgetDataFromJson(BudgetData& data, const QString& file_path);

#endif // DB_RW_H
