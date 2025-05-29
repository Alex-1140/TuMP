#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    client_(new DbClient(this))
{
    ui->setupUi(this);

    // Привязываем слоты клиента к GUI
    connect(client_, &DbClient::responseReceived, this, &MainWindow::handleResponse);
    connect(client_, &DbClient::errorOccurred, this, &MainWindow::handleError);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setClient(DbClient* client) {
    client_ = client;
    connect(client_, &DbClient::responseReceived, this, &MainWindow::handleResponse);
    connect(client_, &DbClient::errorOccurred, this, &MainWindow::handleError);
}

void MainWindow::on_sendButton_clicked() {
    QString cmd = ui->commandLineEdit->text();
    client_->sendCommand(cmd);
}

void MainWindow::handleResponse(const QString& resp) {
    ui->logTextEdit->appendPlainText("[S] " + resp);
}

void MainWindow::handleError(const QString& err) {
    ui->logTextEdit->appendPlainText("[E] " + err);
}
