// mainwindow.h
#pragma once

#include <QMainWindow>
class QTextEdit;
class QLineEdit;
class QPushButton;
class DbClient; // добавим ссылку на клиента

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(DbClient* client, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void sendCommand();
    void handleResponse(const QString& resp);

private:
    QTextEdit* consoleOutput_;
    QLineEdit* commandInput_;
    QPushButton* sendBtn_;
    DbClient* client_;
};