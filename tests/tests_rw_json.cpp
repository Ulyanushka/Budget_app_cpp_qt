#include "tests_rw_json.h"

#include <QtTest>
#include "../db/rw.h"

#include <QTest>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDate>


void TestRwJson::TestWritePeriodToJson()
{
    Period p;
    p.start = QDate(2026, 1, 1);
    p.end   = QDate(2026, 12, 31);

    QJsonObject root;
    WritePeriodToJson(root, p);

    QVERIFY(root.contains("period"));
    QVERIFY(root["period"].isObject());
    QJsonObject obj = root["period"].toObject();
    QCOMPARE(obj["start"].toString(), QString("2026-01-01"));
    QCOMPARE(obj["end"].toString(),   QString("2026-12-31"));
}

void TestRwJson::TestReadPeriodFromJson_valid()
{
    QJsonObject root;
    QJsonObject periodObj;
    periodObj["start"] = "2025-06-15";
    periodObj["end"]   = "2025-07-15";
    root["period"] = periodObj;

    Period p;
    QVERIFY(ReadPeriodFromJson(root, p));
    QCOMPARE(p.start, QDate(2025, 6, 15));
    QCOMPARE(p.end,   QDate(2025, 7, 15));
}

void TestRwJson::TestReadPeriodFromJson_missing()
{
    QJsonObject root;
    Period p;
    QVERIFY(!ReadPeriodFromJson(root, p));
}

void TestRwJson::TestReadPeriodFromJson_notObject()
{
    QJsonObject root;
    root["period"] = "not an object";
    Period p;
    QVERIFY(!ReadPeriodFromJson(root, p));
}

void TestRwJson::TestWriteIncomesToJson()
{
    QVector<Income> incomes = {
        {"Salary", 5000},
        {"Freelance", 1200}
    };

    QJsonObject root;
    WriteIncomesToJson(root, incomes);

    QVERIFY(root.contains("incomes"));
    QJsonArray arr = root["incomes"].toArray();
    QCOMPARE(arr.size(), 2);
    QCOMPARE(arr[0].toObject()["source"].toString(), QString("Salary"));
    QCOMPARE(arr[0].toObject()["amount"].toInt(), 5000);
    QCOMPARE(arr[1].toObject()["source"].toString(), QString("Freelance"));
    QCOMPARE(arr[1].toObject()["amount"].toInt(), 1200);
}

void TestRwJson::TestWriteIncomesToJson_empty()
{
    QVector<Income> incomes;
    QJsonObject root;
    WriteIncomesToJson(root, incomes);
    QVERIFY(root.contains("incomes"));
    QVERIFY(root["incomes"].toArray().isEmpty());
}

void TestRwJson::TestReadIncomesFromJson_valid()
{
    QJsonObject root;
    QJsonArray arr;
    {
        QJsonObject obj;
        obj["source"] = "Dividends";
        obj["amount"] = 300;
        arr.append(obj);
    }
    root["incomes"] = arr;

    QVector<Income> incomes;
    QVERIFY(ReadIncomesFromJson(root, incomes));
    QCOMPARE(incomes.size(), 1);
    QCOMPARE(incomes[0].source, QString("Dividends"));
    QCOMPARE(incomes[0].amount, 300);
}

void TestRwJson::TestReadIncomesFromJson_missing()
{
    QJsonObject root;
    QVector<Income> incomes;
    QVERIFY(ReadIncomesFromJson(root, incomes));
    QVERIFY(incomes.isEmpty());
}

void TestRwJson::TestReadIncomesFromJson_notArray()
{
    QJsonObject root;
    root["incomes"] = "wrong type";
    QVector<Income> incomes;
    QVERIFY(!ReadIncomesFromJson(root, incomes));
}

void TestRwJson::TestWriteExpensesToJson_simple()
{
    Expense exp;
    exp.article = "Food";
    exp.amounts = {300, 450};

    QJsonObject root;
    WriteExpensesToJson(root, {exp});

    QJsonArray arr = root["expenses"].toArray();
    QCOMPARE(arr.size(), 1);
    QJsonObject obj = arr[0].toObject();
    QCOMPARE(obj["article"].toString(), QString("Food"));
    QJsonArray amounts = obj["amounts"].toArray();
    QCOMPARE(amounts.size(), 2);
    QCOMPARE(amounts[0].toInt(), 300);
    QCOMPARE(amounts[1].toInt(), 450);
    QVERIFY(!obj.contains("comments"));
}

void TestRwJson::TestWriteExpensesToJson_withComments()
{
    Expense exp;
    exp.article = "Transport";
    exp.amounts = {100, 200};
    exp.comments = {"Bus", "Taxi"};

    QJsonObject root;
    WriteExpensesToJson(root, {exp});

    QJsonObject obj = root["expenses"].toArray()[0].toObject();
    QJsonArray comments = obj["comments"].toArray();
    QCOMPARE(comments.size(), 2);
    QCOMPARE(comments[0].toString(), QString("Bus"));
    QCOMPARE(comments[1].toString(), QString("Taxi"));
}

void TestRwJson::TestWriteExpensesToJson_mismatchedComments()
{
    Expense exp;
    exp.article = "Mismatch";
    exp.amounts = {10, 20};
    exp.comments = {"only one comment"}; // длина не совпадает

    QJsonObject root;
    WriteExpensesToJson(root, {exp});
    QJsonObject obj = root["expenses"].toArray()[0].toObject();
    QVERIFY(!obj.contains("comments"));
}

void TestRwJson::TestWriteExpensesToJson_emptyCommentElement()
{
    Expense exp;
    exp.article = "EmptyComment";
    exp.amounts = {1};
    exp.comments = {""}; // пустая строка

    QJsonObject root;
    WriteExpensesToJson(root, {exp});
    QJsonObject obj = root["expenses"].toArray()[0].toObject();
    QVERIFY(!obj.contains("comments"));
}

void TestRwJson::TestReadExpensesFromJson_basic()
{
    QJsonObject root;
    QJsonArray arr;
    {
        QJsonObject obj;
        obj["article"] = "Rent";
        obj["amounts"] = QJsonArray{1200};
        arr.append(obj);
    }
    root["expenses"] = arr;

    QVector<Expense> expenses;
    QVERIFY(ReadExpensesFromJson(root, expenses));
    QCOMPARE(expenses.size(), 1);
    QCOMPARE(expenses[0].article, QString("Rent"));
    QCOMPARE(expenses[0].amounts, QVector<int>({1200}));
    QVERIFY(expenses[0].comments.isEmpty());
}

void TestRwJson::TestReadExpensesFromJson_withComments()
{
    QJsonObject root;
    QJsonArray arr;
    {
        QJsonObject obj;
        obj["article"] = "Fun";
        obj["amounts"] = QJsonArray{20, 30};
        obj["comments"] = QJsonArray{"Cinema", "Coffee"};
        arr.append(obj);
    }
    root["expenses"] = arr;

    QVector<Expense> expenses;
    QVERIFY(ReadExpensesFromJson(root, expenses));
    QCOMPARE(expenses[0].comments, QVector<QString>({"Cinema", "Coffee"}));
}

void TestRwJson::TestReadExpensesFromJson_mismatchedComments()
{
    QJsonObject root;
    QJsonArray arr;
    {
        QJsonObject obj;
        obj["article"] = "Bad";
        obj["amounts"] = QJsonArray{1, 2, 3};
        obj["comments"] = QJsonArray{"A", "B"}; // длина 2 ≠ 3
        arr.append(obj);
    }
    root["expenses"] = arr;

    QVector<Expense> expenses;
    QVERIFY(ReadExpensesFromJson(root, expenses));
    QVERIFY(expenses[0].comments.isEmpty());
}

void TestRwJson::TestReadExpensesFromJson_emptyCommentInArray()
{
    QJsonObject root;
    QJsonArray arr;
    {
        QJsonObject obj;
        obj["article"] = "Empty";
        obj["amounts"] = QJsonArray{10};
        obj["comments"] = QJsonArray{""};
        arr.append(obj);
    }
    root["expenses"] = arr;

    QVector<Expense> expenses;
    QVERIFY(ReadExpensesFromJson(root, expenses));
    QVERIFY(expenses[0].comments.isEmpty());
}

void TestRwJson::TestReadExpensesFromJson_notArray()
{
    QJsonObject root;
    root["expenses"] = 42;
    QVector<Expense> expenses;
    QVERIFY(!ReadExpensesFromJson(root, expenses));
}

void TestRwJson::TestReadExpensesFromJson_missing()
{
    QJsonObject root;
    QVector<Expense> expenses;
    QVERIFY(ReadExpensesFromJson(root, expenses));
    QVERIFY(expenses.isEmpty());
}

void TestRwJson::TestFullRoundTrip()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    BudgetData original;
    original.name = "TestBudget";
    original.period.start = QDate(2024, 2, 1);
    original.period.end   = QDate(2024, 2, 28);
    original.incomes = {{"Job", 3000}, {"Gift", 200}};
    original.expenses = {
        {"Housing", {1000}, {"Rent"}},
        {"Food",    {400, 350}, {"Groceries", "Dining out"}}
    };

    QVERIFY(WriteBudgetDataToJson(original, temp_dir.path()));

    QFileInfo fi(temp_dir.path() + "/TestBudget.json");
    QVERIFY(fi.exists());

    BudgetData loaded;
    QVERIFY(ReadBudgetDataFromJson(loaded, fi.absoluteFilePath()));

    QCOMPARE(loaded.name, original.name);
    QCOMPARE(loaded.period.start, original.period.start);
    QCOMPARE(loaded.period.end,   original.period.end);
    QCOMPARE(loaded.incomes.size(), original.incomes.size());
    for (int i = 0; i < loaded.incomes.size(); ++i) {
        QCOMPARE(loaded.incomes[i].source, original.incomes[i].source);
        QCOMPARE(loaded.incomes[i].amount, original.incomes[i].amount);
    }
    QCOMPARE(loaded.expenses.size(), original.expenses.size());
    for (int i = 0; i < loaded.expenses.size(); ++i) {
        QCOMPARE(loaded.expenses[i].article, original.expenses[i].article);
        QCOMPARE(loaded.expenses[i].amounts, original.expenses[i].amounts);
        QCOMPARE(loaded.expenses[i].comments, original.expenses[i].comments);
    }
}

void TestRwJson::TestWriteEmptyName()
{
    QTemporaryDir temp_dir;
    BudgetData data;
    data.name = "   ";
    QVERIFY(!WriteBudgetDataToJson(data, temp_dir.path()));
}

void TestRwJson::TestReadNameMismatch()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    QJsonObject root;
    root["name"] = "Conflict";
    root["period"] = QJsonObject{{"start", "2026-01-01"}, {"end", "2026-01-31"}};
    root["incomes"] = QJsonArray();
    root["expenses"] = QJsonArray();
    QJsonDocument doc(root);

    QFile file(temp_dir.path() + "/External.json");
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write(doc.toJson());
    file.close();

    BudgetData data;
    QVERIFY(!ReadBudgetDataFromJson(data, temp_dir.path() + "/External.json"));
}

void TestRwJson::TestReadInvalidJson()
{
    QTemporaryFile tmp_file("XXXXXX.json");
    QVERIFY(tmp_file.open());
    tmp_file.write("this is not a json");
    tmp_file.close();

    BudgetData data;
    QVERIFY(!ReadBudgetDataFromJson(data, tmp_file.fileName()));
}

void TestRwJson::TestReadRootNotObject()
{
    QTemporaryFile tmp_file("XXXXXX.json");
    QVERIFY(tmp_file.open());
    tmp_file.write("[1,2,3]");
    tmp_file.close();

    BudgetData data;
    QVERIFY(!ReadBudgetDataFromJson(data, tmp_file.fileName()));
}

void TestRwJson::TestReadMissingPeriod()
{
    QTemporaryFile tmp_file("XXXXXX.json");
    QVERIFY(tmp_file.open());
    QJsonObject root;
    root["name"] = "Test";
    root["incomes"] = QJsonArray();
    root["expenses"] = QJsonArray();
    QJsonDocument doc(root);
    tmp_file.write(doc.toJson());
    tmp_file.close();

    BudgetData data;
    QVERIFY(!ReadBudgetDataFromJson(data, tmp_file.fileName()));
}

void TestRwJson::TestReadEmptyIncomesExpenses()
{
    QTemporaryDir temp_dir;
    QVERIFY(temp_dir.isValid());

    BudgetData data;
    data.name = "Sparse";
    data.period.start = QDate(2026, 6, 1);
    data.period.end   = QDate(2026, 6, 30);

    QVERIFY(WriteBudgetDataToJson(data, temp_dir.path()));

    BudgetData loaded;
    QVERIFY(ReadBudgetDataFromJson(loaded, temp_dir.path() + "/Sparse.json"));
    QCOMPARE(loaded.name, data.name);
    QCOMPARE(loaded.period.start, data.period.start);
    QVERIFY(loaded.incomes.isEmpty());
    QVERIFY(loaded.expenses.isEmpty());
}

void TestRwJson::TestCannotOpenFileForWriting()
{
    BudgetData data;
    data.name = "FailWrite";
    QVERIFY(!WriteBudgetDataToJson(data, "/this/dir/does/not/exist"));
}

void TestRwJson::TestCannotOpenFileForReading()
{
    BudgetData data;
    QVERIFY(!ReadBudgetDataFromJson(data, "/nonexistent/path.json"));
}
