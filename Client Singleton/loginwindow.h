#pragma once

#include <QDialog>
class QLineEdit;
class QPushButton;
class DbClient;

class LoginWindow : public QDialog {
    Q_OBJECT
public:
    explicit LoginWindow(DbClient* client, QWidget* parent = nullptr);
    ~LoginWindow();

private slots:
    void onConnectClicked();
    void onLoginClicked();
    void onRegisterClicked();
    void handleResponse(const QString& response);
    void handleError(const QString& error);
    void handleConnected();

private:
    QLineEdit* hostEdit_;
    QLineEdit* portEdit_;
    QLineEdit* usernameEdit_;
    QLineEdit* passwordEdit_;
    QPushButton* connectBtn_;
    QPushButton* loginBtn_;
    QPushButton* registerBtn_;

    DbClient* client_;
};