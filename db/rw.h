#ifndef DB_RW_H
#define DB_RW_H

#include "QString"

struct BudgetData;

bool WriteBudgetDataToJson(const BudgetData& data, const QString& dir_path = ".");
bool ReadBudgetDataFromJson(BudgetData& data, const QString& file_path);

#endif // DB_RW_H
