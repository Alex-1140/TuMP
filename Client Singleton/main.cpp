#include "mainwindow.h"
#include "loginwindow.h"
#include "dbclient.h"
#include <QApplication>
/*
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DbClient client;
    LoginWindow loginDlg(&client);
    
    if (loginDlg.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow mainWin;
    mainWin.setClient(&client);
    mainWin.show();
    return app.exec();
}
*/
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    DbClient client;
    LoginWindow login(&client);
    login.setWindowTitle("Login");

    int result = login.exec();
    if (result == QDialog::Accepted) {
        MainWindow* mainWindow = new MainWindow(&client);
        mainWindow->show();
        return app.exec();
    }

    return 0;
}