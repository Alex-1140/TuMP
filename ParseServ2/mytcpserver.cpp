#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer(QObject *parent) : QTcpServer(parent) {}

void MyTcpServer::startServer()
{
    if (!this->listen(QHostAddress::LocalHost, 12345)) {
        qCritical() << "Server error:" << this->errorString();
    } else {
        qDebug() << "Server started on port 12345";
    }
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    connect(clientSocket, &QTcpSocket::readyRead, this, &MyTcpServer::readClientData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &MyTcpServer::clientDisconnected);

    sendWelcomeMessage(clientSocket);
}

void MyTcpServer::readClientData()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    while (clientSocket->canReadLine()) {
        QString line = QString::fromUtf8(clientSocket->readLine()).trimmed();
        processCommand(clientSocket, line);
    }
}

void MyTcpServer::clientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        clientSocket->deleteLater();
    }
}

QMap<int, double> MyTcpServer::findShortestPaths(int src)
{
    QMap<int, double> distances;
    QSet<int> visited;
    QVector<int> nodes = getUniqueNodes();

    // Инициализация
    for (int node : nodes) {
        distances[node] = std::numeric_limits<double>::infinity();
    }
    distances[src] = 0;

    while (visited.size() < nodes.size()) {
        // Находим узел с минимальным расстоянием
        int current = -1;
        double minDist = std::numeric_limits<double>::infinity();

        for (int node : nodes) {
            if (!visited.contains(node) && distances[node] < minDist) {
                minDist = distances[node];
                current = node;
            }
        }

        if (current == -1) break;

        visited.insert(current);

        // Обновляем расстояния до соседей
        for (auto it = graph[current].constBegin(); it != graph[current].constEnd(); ++it) {
            int neighbor = it.key();
            double weight = it.value();
            double newDist = distances[current] + weight;

            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
            }
        }
    }

    return distances;
}

QVector<int> MyTcpServer::getUniqueNodes()
{
    QSet<int> nodes;
    for (auto it = graph.constBegin(); it != graph.constEnd(); ++it) {
        nodes.insert(it.key());
        for (auto neighbor : it.value().keys()) {
            nodes.insert(neighbor);
        }
    }
    return nodes.values().toVector();
}

void MyTcpServer::sendWelcomeMessage(QTcpSocket *socket)
{
    QString message =
        "Добро пожаловать в Graph Server!\n"
        "Доступные команды:\n"
        "ADD_EDGE u v w    - Добавить ребро\n"
        "SHORTEST_PATH src - Кратчайшие пути\n"
        "QUIT              - Выход\n"
        "Введите команду: ";
    socket->write(message.toUtf8());
}

void MyTcpServer::processCommand(QTcpSocket *socket, const QString &commandLine)
{
    QStringList parts = commandLine.split(" ", Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        socket->write("ERROR: Empty command\n");
        return;
    }

    QString cmd = parts[0].toUpper();

    if (cmd == "ADD_EDGE") {
        if (parts.size() != 4) {
            socket->write("ERROR: ADD_EDGE requires 3 arguments\n");
        } else {
            bool ok1, ok2, ok3;
            int u = parts[1].toInt(&ok1);
            int v = parts[2].toInt(&ok2);
            double w = parts[3].toDouble(&ok3);

            if (ok1 && ok2 && ok3) {
                graph[u][v] = w;
                graph[v][u] = w; // Для неориентированного графа
                socket->write(QString("OK: Added %1-%2 (%3)\n")
                                  .arg(u).arg(v).arg(w).toUtf8());
            } else {
                socket->write("ERROR: Invalid arguments\n");
            }
        }
    }
    else if (cmd == "SHORTEST_PATH") {
        if (parts.size() != 2) {
            socket->write("ERROR: SHORTEST_PATH requires 1 argument\n");
        } else {
            bool ok;
            int src = parts[1].toInt(&ok);
            if (ok) {
                QMap<int, double> distances = findShortestPaths(src);
                QString response = "DISTANCES from " + QString::number(src) + ":\n";
                for (auto it = distances.constBegin(); it != distances.constEnd(); ++it) {
                    if (it.value() != std::numeric_limits<double>::infinity()) {
                        response += QString("[%1: %2] ").arg(it.key()).arg(it.value());
                    }
                }
                socket->write(response.toUtf8());
            } else {
                socket->write("ERROR: Invalid source node\n");
            }
        }
    }
    else if (cmd == "QUIT") {
        socket->write("BYE\n");
        socket->disconnectFromHost();
    }
    else {
        socket->write("ERROR: Unknown command\n");
    }

    if (cmd != "QUIT") {
        socket->write("\nВведите команду: ");
    }
}
