#include "tests_mainwindow.h"
#include "tests_rw_json.h"

#include <QTest>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    int status = 0;
    status |= QTest::qExec(new TestMainWindow, argc, argv);
    status |= QTest::qExec(new TestRwJson, argc, argv);
    return status;
}
