#include "rw.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDate>

#include "structs.h"


bool WriteBudgetDataToJson(const BudgetData& data, const QString& dir_path) {
    if (data.name.trimmed().isEmpty()) {
        qWarning("Name isempty, can't write the file!");
        return false;
    }
    QDir dir(dir_path);
    QString file_name = data.name + ".json";
    QString file_path = dir.filePath(file_name);

    QJsonObject root;
    root["name"] = data.name;

    QJsonObject period_obj;
    period_obj["start"] = data.period.start.toString(Qt::ISODate);
    period_obj["end"]   = data.period.end.toString(Qt::ISODate);
    root["period"] = period_obj;

    QJsonArray incomes_ar;
    for (const Income& inc : data.incomes) {
        QJsonObject inc_obj;
        inc_obj["source"] = inc.source;
        inc_obj["amount"] = inc.amount;
        incomes_ar.append(inc_obj);
    }
    root["incomes"] = incomes_ar;

    QJsonArray expenses_ar;
    for (const Expense& exp : data.expenses) {
        QJsonObject exp_obj;
        exp_obj["article"] = exp.article;

        QJsonArray amounts_ar;
        for (int val : exp.amounts) {
            amounts_ar.append(val);
        }
        exp_obj["amounts"] = amounts_ar;

        if (!exp.comments.isEmpty()) {
            bool valid = (exp.comments.size() == exp.amounts.size());
            if (valid) {
                for (const QString& c : exp.comments) {
                    if (c.trimmed().isEmpty()) {
                        valid = false;
                        break;
                    }
                }
            }

            if (valid) {
                QJsonArray comments_ar;
                for (const QString &c : exp.comments) {
                    comments_ar.append(c);
                }
                exp_obj["comments"] = comments_ar;
            } else {
                qWarning() << "Article " << exp.article
                           << ": can't write the comments "
                              "(wrong length or empty elements)!";
            }
        }

        expenses_ar.append(exp_obj);
    }
    root["expenses"] = expenses_ar;

    QJsonDocument doc(root);
    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Can'topen the file for writing: %s!", qPrintable(file_path));
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool ReadBudgetDataFromJson(BudgetData& data, const QString& file_path) {
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Can'topen the file for reading: %s!", qPrintable(file_path));
        return false;
    }

    QByteArray raw_data = file.readAll();
    file.close();

    QJsonParseError parse_error;
    QJsonDocument doc = QJsonDocument::fromJson(raw_data, &parse_error);
    if (parse_error.error != QJsonParseError::NoError) {
        qWarning("JSONparsing error: %s!",
                 qPrintable(parse_error.errorString()));
        return false;
    }

    if (!doc.isObject()) {
        qWarning("Root is not an object!");
        return false;
    }

    QJsonObject root = doc.object();
    data.name = root["name"].toString();

    QFileInfo fi(file_path);
    QString expected_name = fi.completeBaseName();
    if (data.name != expected_name) {
        qWarning() << "Name in the file ("
                   << data.name
                   << ") is not same as the file's name ("
                   << expected_name << ")";
        return false;
    }

    if (!root.contains("period") || !root["period"].isObject()) {
        qWarning("Problems with key 'period'");
        return false;
    }
    QJsonObject period_obj = root["period"].toObject();
    data.period.start = QDate::fromString(
        period_obj["start"].toString(), Qt::ISODate);
    data.period.end   = QDate::fromString(
        period_obj["end"].toString(), Qt::ISODate);

    data.incomes.clear();
    if (root.contains("incomes") && root["incomes"].isArray()) {
        QJsonArray incomes_ar = root["incomes"].toArray();
        for (const QJsonValue& val : incomes_ar) {
            QJsonObject obj = val.toObject();
            Income inc;
            inc.source = obj["source"].toString();
            inc.amount = obj["amount"].toInt();
            data.incomes.append(inc);
        }
    }

    data.expenses.clear();
    if (root.contains("expenses") && root["expenses"].isArray()) {
        QJsonArray expenses_ar = root["expenses"].toArray();
        for (const QJsonValue& val : expenses_ar) {
            QJsonObject obj = val.toObject();
            Expense exp;
            exp.article = obj["article"].toString();

            QJsonArray amounts_ar = obj["amounts"].toArray();
            for (const QJsonValue& v : amounts_ar) {
                exp.amounts.append(v.toInt());
            }

            if (obj.contains("comments") && obj["comments"].isArray()) {
                QJsonArray comments_ar = obj["comments"].toArray();
                QVector<QString> loaded_comments;
                bool all_non_empty = true;
                for (const QJsonValue& v : comments_ar) {
                    QString str = v.toString();
                    if (str.trimmed().isEmpty()) {
                        all_non_empty = false;
                        break;
                    }
                    loaded_comments.append(str);
                }

                if (all_non_empty
                    && loaded_comments.size() == exp.amounts.size())
                {
                    exp.comments = loaded_comments;
                } else {
                    qWarning() << "Article " << exp.article
                               << ": can't read the comments "
                                  "(wrong length or empty elements)!";
                }
            }

            data.expenses.append(exp);
        }
    }

    return true;
}
