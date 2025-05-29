#pragma once

#include <QDialog>
#include <QString>

class QLineEdit;
class QPushButton;
class DbClient;

class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(DbClient* client, QWidget* parent = nullptr);
    ~LoginWindow() override = default;

signals:
    void loginSuccess();

private slots:
    void onConnectClicked();
    void onLoginClicked();
    void onRegisterClicked();
    void handleResponse(const QString& resp);
    void handleError(const QString& err);

private:
    DbClient* client_;

    // UI элементы
    QLineEdit* ipEdit_;
    QLineEdit* portEdit_;
    QPushButton* connectBtn_;

    QLineEdit* usernameEdit_;
    QLineEdit* passwordEdit_;
    QPushButton* loginBtn_;
    QPushButton* registerBtn_;
};