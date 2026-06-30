#ifndef TESTS_RW_JSON_H
#define TESTS_RW_JSON_H

#include <QObject>


class TestRwJson : public QObject
{
    Q_OBJECT

private slots:
    void TestWritePeriodToJson();
    void TestReadPeriodFromJson_valid();
    void TestReadPeriodFromJson_missing();
    void TestReadPeriodFromJson_notObject();

    void TestWriteIncomesToJson();
    void TestWriteIncomesToJson_empty();
    void TestReadIncomesFromJson_valid();
    void TestReadIncomesFromJson_missing();
    void TestReadIncomesFromJson_notArray();

    void TestWriteExpensesToJson_simple();
    void TestWriteExpensesToJson_withComments();
    void TestWriteExpensesToJson_mismatchedComments();
    void TestWriteExpensesToJson_emptyCommentElement();
    void TestReadExpensesFromJson_basic();
    void TestReadExpensesFromJson_withComments();
    void TestReadExpensesFromJson_mismatchedComments();
    void TestReadExpensesFromJson_emptyCommentInArray();
    void TestReadExpensesFromJson_notArray();
    void TestReadExpensesFromJson_missing();

    void TestFullRoundTrip();
    void TestWriteEmptyName();
    void TestReadNameMismatch();
    void TestReadInvalidJson();
    void TestReadRootNotObject();
    void TestReadMissingPeriod();
    void TestReadEmptyIncomesExpenses();
    void TestCannotOpenFileForWriting();
    void TestCannotOpenFileForReading();
};

#endif // TESTS_RW_JSON_H
