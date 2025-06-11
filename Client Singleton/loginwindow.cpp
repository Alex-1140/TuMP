#include "loginwindow.h"
#include "dbclient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

LoginWindow::LoginWindow(DbClient* client, QWidget* parent) : QDialog(parent), client_(client) {
    setWindowTitle("Client Login");

    hostEdit_ = new QLineEdit("127.0.0.1");
    portEdit_ = new QLineEdit("12345");
    usernameEdit_ = new QLineEdit;
    passwordEdit_ = new QLineEdit;
    passwordEdit_->setEchoMode(QLineEdit::Password);

    connectBtn_ = new QPushButton("Connect");
    loginBtn_ = new QPushButton("Login");
    registerBtn_ = new QPushButton("Register");

    loginBtn_->setEnabled(false);
    registerBtn_->setEnabled(false);
    usernameEdit_->setEnabled(false);
    passwordEdit_->setEnabled(false);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow("Host:", hostEdit_);
    formLayout->addRow("Port:", portEdit_);
    formLayout->addRow("Username:", usernameEdit_);
    formLayout->addRow("Password:", passwordEdit_);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(connectBtn_);
    buttonLayout->addWidget(loginBtn_);
    buttonLayout->addWidget(registerBtn_);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(connectBtn_, &QPushButton::clicked, this, &LoginWindow::onConnectClicked);
    connect(loginBtn_, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(registerBtn_, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);

    connect(client_, &DbClient::responseReceived, this, &LoginWindow::handleResponse);
    connect(client_, &DbClient::errorOccurred, this, &LoginWindow::handleError);
    connect(client_, &DbClient::connected, this, &LoginWindow::handleConnected);
}

LoginWindow::~LoginWindow() {}

void LoginWindow::onConnectClicked() {
    const QString host = hostEdit_->text().trimmed();
    const quint16 port = portEdit_->text().toUShort();
    client_->connectToServer(host, port);
}

void LoginWindow::onLoginClicked() {
    const QString u = usernameEdit_->text().trimmed();
    const QString p = passwordEdit_->text().trimmed();
    if (u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Username and password cannot be empty.");
        return;
    }
    client_->sendCommand(QString("LOGIN %1 %2").arg(u, p));
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

void LoginWindow::handleResponse(const QString& resp) {
    qDebug() << "[LoginWindow] Got response:" << resp;
    if (resp.trimmed() == "OK: Login successful") {
        qDebug() << "[LoginWindow] Login success!";
        accept();
    }
    else if (resp.startsWith("OK: Registration successful")) {
        QMessageBox::information(this, "Registration", "User successfully registered.");
    }
    else if (resp.startsWith("ERROR")) {
        QMessageBox::critical(this, "Server Error", resp);
    }
    else {
        QMessageBox::information(this, "Response", resp);
    }
}

void LoginWindow::handleError(const QString& error) {
    QMessageBox::critical(this, "Client Error", error);
}

void LoginWindow::handleConnected() {
    qDebug() << "[LoginWindow] Connected to server!";
    QMessageBox::information(this, "Connected", "Successfully connected to server.");
    usernameEdit_->setEnabled(true);
    passwordEdit_->setEnabled(true);
    loginBtn_->setEnabled(true);
    registerBtn_->setEnabled(true);
    connectBtn_->setEnabled(false);
}