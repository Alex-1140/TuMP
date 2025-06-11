#include <QCoreApplication>
#include "test_save_db.h"

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    testPasswordHashStored();
    return 0;
}
