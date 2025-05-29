#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbclient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setClient(DbClient* client);

private slots:
//    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void handleResponse(const QString& resp);
    void handleError(const QString& err);

private:
    Ui::MainWindow* ui;
    DbClient* client_;
};

#endif // MAINWINDOW_H
