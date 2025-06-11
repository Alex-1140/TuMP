// mainwindow.cpp
#include "mainwindow.h"
#include "dbclient.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QDebug>

MainWindow::MainWindow(DbClient* client, QWidget* parent) : QMainWindow(parent), client_(client) {
    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout;

    consoleOutput_ = new QTextEdit;
    consoleOutput_->setReadOnly(true);

    QHBoxLayout* inputLayout = new QHBoxLayout;
    commandInput_ = new QLineEdit;
    sendBtn_ = new QPushButton("Send");
    inputLayout->addWidget(commandInput_);
    inputLayout->addWidget(sendBtn_);

    mainLayout->addWidget(consoleOutput_);
    mainLayout->addLayout(inputLayout);

    central->setLayout(mainLayout);
    setCentralWidget(central);
    setWindowTitle("Command Console");
    resize(600, 400);

    connect(sendBtn_, &QPushButton::clicked, this, &MainWindow::sendCommand);
    connect(client_, &DbClient::responseReceived, this, &MainWindow::handleResponse);
    connect(client_, &DbClient::errorOccurred, this, [](const QString& err) {
        QMessageBox::critical(nullptr, "Client Error", err);
        });
}

MainWindow::~MainWindow() {}

void MainWindow::sendCommand() {
    QString cmd = commandInput_->text();
    if (cmd.isEmpty()) return;
    consoleOutput_->append("> " + cmd);
    client_->sendCommand(cmd); // теперь отправляем на сервер
    commandInput_->clear();
}

void MainWindow::handleResponse(const QString& resp) {
    consoleOutput_->append("< " + resp);
}
