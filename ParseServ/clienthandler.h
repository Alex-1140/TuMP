#pragma once
#include <QTcpSocket>
#include "GraphProcessor.h"
#include "DatabaseManager.h"

class ClientHandler : public QObject {
    Q_OBJECT

public:
    ClientHandler(QTcpSocket* socket, QObject* parent = nullptr);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket* socket;
    GraphProcessor graphProcessor;
    DatabaseManager dbManager;

    void processRequest(const QString& data);
};
