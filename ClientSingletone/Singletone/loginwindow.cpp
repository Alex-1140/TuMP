#include "loginwindow.h"
#include "dbclient.h"

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

LoginWindow::LoginWindow(DbClient* client, QWidget* parent)
    : QDialog(parent), client_(client)
{
    setWindowTitle("Connect & Login");
    setModal(true);

    // --- Секция подключения ---  
    auto* lblIp = new QLabel("Server IP:", this);
    ipEdit_ = new QLineEdit(this);
    ipEdit_->setText("127.0.0.1");

    auto* lblPort = new QLabel("Port:", this);
    portEdit_ = new QLineEdit(this);
    portEdit_->setText("8080");

    connectBtn_ = new QPushButton("Connect", this);

    // --- Секция авторизации ---  
    auto* lblUser = new QLabel("Username:", this);
    usernameEdit_ = new QLineEdit(this);

    auto* lblPass = new QLabel("Password:", this);
    passwordEdit_ = new QLineEdit(this);
    passwordEdit_->setEchoMode(QLineEdit::Password);

    loginBtn_ = new QPushButton("Login", this);
    registerBtn_ = new QPushButton("Register", this);

    // --- Компоновка ---  
    auto* connLayout = new QGridLayout;
    connLayout->addWidget(lblIp, 0, 0);
    connLayout->addWidget(ipEdit_, 0, 1);
    connLayout->addWidget(lblPort, 1, 0);
    connLayout->addWidget(portEdit_, 1, 1);
    connLayout->addWidget(connectBtn_, 2, 0, 1, 2);

    auto* authLayout = new QGridLayout;
    authLayout->addWidget(lblUser, 0, 0);
    authLayout->addWidget(usernameEdit_, 0, 1);
    authLayout->addWidget(lblPass, 1, 0);
    authLayout->addWidget(passwordEdit_, 1, 1);
    authLayout->addWidget(loginBtn_, 2, 0);
    authLayout->addWidget(registerBtn_, 2, 1);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(connLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(authLayout);
    setLayout(mainLayout);

    // --- Сигналы ---  
    connect(connectBtn_, &QPushButton::clicked, this, &LoginWindow::onConnectClicked);
    connect(loginBtn_, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(registerBtn_, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(client_, &DbClient::responseReceived, this, &LoginWindow::handleResponse);
    connect(client_, &DbClient::errorOccurred, this, &LoginWindow::handleError);
    connect(loginBtn_, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);

    // По умолчанию, отключаем авторизацию до подключения
    usernameEdit_->setEnabled(false);
    passwordEdit_->setEnabled(false);
    loginBtn_->setEnabled(false);
    registerBtn_->setEnabled(false);
}

void LoginWindow::onConnectClicked() {
    const QString ip = ipEdit_->text().trimmed();
    const quint16 port = portEdit_->text().toUShort();
    if (ip.isEmpty() || port == 0) {
        QMessageBox::warning(this, "Input Error", "Enter valid IP and port.");
        return;
    }
    client_->connectToServer(ip, port);

    // Активируем авторизацию после успешного подключения
    // (можно также ждать сигнала connected(), но здесь упрощенно)
    usernameEdit_->setEnabled(true);
    passwordEdit_->setEnabled(true);
    loginBtn_->setEnabled(true);
    registerBtn_->setEnabled(true);
    connectBtn_->setEnabled(false);
}

void LoginWindow::onLoginClicked() {
    const QString u = usernameEdit_->text().trimmed();
    const QString p = passwordEdit_->text().trimmed();
    if (u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Username and password cannot be empty.");
        return;
    }
    qDebug() << "Sending LOGIN command";
    client_->sendCommand(QString("LOGIN %1 %2").arg(u, p));
}

void LoginWindow::handleResponse(const QString& resp) {
    qDebug() << "Got response:" << resp;
    if (resp.trimmed() == "LOGIN OK") {
        qDebug() << "Login success!";
        accept();
    }
}

void LoginWindow::onRegisterClicked() {
    const QString u = usernameEdit_->text().trimmed();
    const QString p = passwordEdit_->text().trimmed();
    if (u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Username and password cannot be empty.");
        return;
    }
    client_->sendCommand(QString("REGISTER %1 %2").arg(u, p));
}

void LoginWindow::handleError(const QString& err) {
    QMessageBox::critical(this, "Network Error", err);
}
