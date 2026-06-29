#include "rw.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDate>


void WritePeriodToJson(QJsonObject& json, const Period& period) {
    QJsonObject obj;
    obj["start"] = period.start.toString(Qt::ISODate);
    obj["end"]   = period.end.toString(Qt::ISODate);
    json["period"] = obj;
}

void WriteIncomesToJson(QJsonObject& json, const QVector<Income>& incomes) {
    QJsonArray income_arr;
    for (const Income& inc : incomes) {
        QJsonObject obj;
        obj["source"] = inc.source;
        obj["amount"] = inc.amount;
        income_arr.append(obj);
    }
    json["incomes"] = income_arr;
}

void WriteExpensesToJson(QJsonObject& json, const QVector<Expense>& expenses) {
    QJsonArray expense_arr;
    for (const Expense& exp : expenses) {
        QJsonObject obj;
        obj["article"] = exp.article;

        QJsonArray amount_arr;
        for (int val : exp.amounts) {
            amount_arr.append(val);
        }
        obj["amounts"] = amount_arr;

        if (!exp.comments.isEmpty()) {
            bool valid = (exp.comments.size() == exp.amounts.size());
            if (valid) {
                for (const QString& comment : exp.comments) {
                    if (comment.trimmed().isEmpty()) {
                        valid = false;
                        break;
                    }
                }
            }
            if (valid) {
                QJsonArray comment_arr;
                for (const QString& comment : exp.comments)
                    comment_arr.append(comment);
                obj["comments"] = comment_arr;
            } else {
                qWarning() << "Article " << exp.article
                           << ": can't write the comments "
                              "(wrong length or empty elements)!";
            }
        }

        expense_arr.append(obj);
    }
    json["expenses"] = expense_arr;
}


bool ReadPeriodFromJson(const QJsonObject& json, Period& period) {
    if (!json.contains("period") || !json["period"].isObject()) {
        return false;
    }

    QJsonObject obj = json["period"].toObject();
    period.start = QDate::fromString(obj["start"].toString(), Qt::ISODate);
    period.end   = QDate::fromString(obj["end"].toString(), Qt::ISODate);
    return true;
}

bool ReadIncomesFromJson(const QJsonObject& json, QVector<Income>& incomes) {
    incomes.clear();
    if (json.contains("incomes")) {
        if (!json["incomes"].isArray())
            return false;

        QJsonArray income_arr = json["incomes"].toArray();
        for (const QJsonValue& val : income_arr) {
            QJsonObject obj = val.toObject();
            Income inc;
            inc.source = obj["source"].toString();
            inc.amount = obj["amount"].toInt();
            incomes.append(inc);
        }
    }
    return true;
}

bool ReadExpensesFromJson(const QJsonObject& json, QVector<Expense>& expenses) {
    expenses.clear();
    if (json.contains("expenses")) {
        if (!json["expenses"].isArray()) {
            return false;
        }

        QJsonArray expense_arr = json["expenses"].toArray();
        for (const QJsonValue& val : expense_arr) {
            QJsonObject obj = val.toObject();
            Expense exp;
            exp.article = obj["article"].toString();

            QJsonArray amount_arr = obj["amounts"].toArray();
            for (const QJsonValue& av : amount_arr) {
                exp.amounts.append(av.toInt());
            }

            if (obj.contains("comments") && obj["comments"].isArray()) {
                QJsonArray comment_arr = obj["comments"].toArray();
                QVector<QString> loaded;
                bool ok = true;
                for (const QJsonValue& cv : comment_arr) {
                    QString str = cv.toString();
                    if (str.trimmed().isEmpty()) {
                        ok = false;
                        break;
                    }
                    loaded.append(str);
                }
                if (ok && loaded.size() == exp.amounts.size()) {
                    exp.comments = loaded;
                } else {
                    qWarning() << "Article " << exp.article
                               << ": can't read the comments "
                                  "(wrong length or empty elements)!";
                }
            }

            expenses.append(exp);
        }
    }
    return true;
}


bool WriteBudgetDataToJson(const BudgetData& data, const QString& dir_path) {
    if (data.name.trimmed().isEmpty()) {
        qWarning("Name is empty, can't write the file!");
        return false;
    }

    QDir dir(dir_path);
    QString file_path = dir.filePath(data.name + ".json");

    QJsonObject root;
    root["name"] = data.name;

    WritePeriodToJson(root, data.period);
    WriteIncomesToJson(root, data.incomes);
    WriteExpensesToJson(root, data.expenses);

    QJsonDocument doc(root);
    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Can't open the file for writing: %s!", qPrintable(file_path));
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool ReadBudgetDataFromJson(BudgetData& data, const QString& file_path) {
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Can't open the file for reading: %s!", qPrintable(file_path));
        return false;
    }

    QByteArray raw_data = file.readAll();
    file.close();

    QJsonParseError parse_error;
    QJsonDocument doc = QJsonDocument::fromJson(raw_data, &parse_error);
    if (parse_error.error != QJsonParseError::NoError) {
        qWarning("JSON parsing error: %s!", qPrintable(parse_error.errorString()));
        return false;
    }
    if (!doc.isObject()) {
        qWarning("Root is not an object!");
        return false;
    }

    QJsonObject root = doc.object();

    data.name = root["name"].toString();
    QFileInfo file_info(file_path);
    if (data.name != file_info.completeBaseName()) {
        qWarning() << "Name in the file (" << data.name
                   << ") is not same as the file's name ("
                   << file_info.completeBaseName() << ")";
        return false;
    }

    if (!ReadPeriodFromJson(root, data.period)) {
        qWarning("Problems with key 'period'");
        return false;
    }

    if (!ReadIncomesFromJson(root, data.incomes)) {
        return false;
    }

    if (!ReadExpensesFromJson(root, data.expenses)) {
        return false;
    }

    return true;
}
