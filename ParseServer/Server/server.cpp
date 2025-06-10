#include "server.h"
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QRegularExpression>
#include <cmath>

Server::Server(QObject* parent) : QObject(parent) {
    std::cout << "Starting server..." << std::endl;
    tcpServer = new QTcpServer(this);
    std::cout << "Server created, connecting signals..." << std::endl;
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::handleNewConnection);
    std::cout << "Attempting to listen on port 12345..." << std::endl;
    if (!tcpServer->listen(QHostAddress::Any, 12345)) {
        std::cout << "Server could not start! Error: " << tcpServer->errorString().toStdString() << std::endl;
        exit(1);
    } else {
        std::cout << "Server started on port 12345" << std::endl;
    }
}

QString Server::vigenereCipher(const QString& text, bool encrypt) {
    QString key = vigenereKey;
    QString result;
    int keyIndex = 0;
    for (int i = 0; i < text.length(); ++i) {
        QChar c = text[i];
        if (!c.isLetter()) {
            result += c;
            continue;
        }
        QChar k = key[keyIndex % key.length()];
        keyIndex++;
        int shift = k.toUpper().toLatin1() - 'A';
        int base = c.isUpper() ? 'A' : 'a';
        int value = c.toLatin1() - base;
        if (encrypt) {
            value = (value + shift) % 26;
        } else {
            value = (value - shift + 26) % 26;
        }
        result += QChar(value + base);
    }
    return result;
}

QString Server::hashWithSha512(const QString& data) {
    return QString(QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha512).toHex());
}

double Server::evaluateFunction(const QString& expr, double x) {
    QString expression = expr;
    expression.replace("x", QString::number(x));
    bool ok;
    double result = expression.toDouble(&ok);
    if (ok) return result;

    // Разбиение с использованием QRegularExpression
    QStringList parts = expression.split(QRegularExpression("(?=[-+*/^])|(?<=[-+*/^])"), Qt::SkipEmptyParts);
    double current = 0.0;
    QString operation = "+";
    for (const QString& part : parts) {
        if (part == "+" || part == "-" || part == "*" || part == "/" || part == "^") {
            operation = part;
            continue;
        }
        double value = part.toDouble(&ok);
        if (!ok) continue; // Игнорируем некорректные части
        if (operation == "+") current += value;
        else if (operation == "-") current -= value;
        else if (operation == "*") current *= value;
        else if (operation == "/") current /= value;
        else if (operation == "^") current = pow(current, value);
    }
    return current;
}

QString Server::halfMethod(double a, double b, double epsilon, const QString& expr) {
    if (b <= a || epsilon <= 0) {
        return "ERROR: Invalid interval or epsilon";
    }
    double fa = evaluateFunction(expr, a);
    double fb = evaluateFunction(expr, b);
    if (fa * fb >= 0) {
        return "ERROR: Function values at endpoints have same sign";
    }

    double c = a;
    while ((b - a) >= epsilon) {
        c = (a + b) / 2;
        double fc = evaluateFunction(expr, c);
        if (fc == 0.0) break; // Точный корень
        if (fa * fc < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }
    }
    return "RESULT: Root ≈ " + QString::number(c, 'f', 6);
}

void Server::handleNewConnection() {
    std::cout << "New connection received..." << std::endl;
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    std::cout << "Client connected from " << clientSocket->peerAddress().toString().toStdString()
              << ":" << clientSocket->peerPort() << std::endl;
    authenticatedUsers[clientSocket] = ""; // Изначально клиент не авторизован

    connect(clientSocket, &QTcpSocket::readyRead, this, [this, clientSocket]() {
        std::cout << "Data received from client... Available bytes: " << clientSocket->bytesAvailable() << std::endl;
        QDataStream in(clientSocket);
        in.setVersion(QDataStream::Qt_5_15);
        QString command;
        in >> command;
        std::cout << "Received command: " << command.toStdString() << std::endl;
        QStringList parts = command.split(" ");
        QString response;

        if (parts[0] == "REGISTER" && parts.size() == 3) {
            QString login = parts[1];
            QString password = parts[2];
            if (checkUser(login, password)) {
                response = "ERROR: User already exists";
            } else {
                saveUser(login, password);
                response = "OK: User registered";
            }
        } else if (parts[0] == "LOGIN" && parts.size() == 3) {
            QString login = parts[1];
            QString password = parts[2];
            std::cout << "Checking user: " << login.toStdString() << " with password: " << password.toStdString() << std::endl;
            if (checkUser(login, password)) {
                authenticatedUsers[clientSocket] = login;
                response = "OK: Login successful";
            } else {
                response = "ERROR: Invalid login or password";
            }
        } else if (!isAuthenticated(clientSocket)) {
            response = "ERROR: Please login first";
        } else if (parts[0] == "ADD_EDGE" && parts.size() == 4) {
            bool ok;
            int u = parts[1].toInt(&ok); if (!ok) { response = "ERROR: Invalid u"; return; }
            int v = parts[2].toInt(&ok); if (!ok) { response = "ERROR: Invalid v"; return; }
            double w = parts[3].toDouble(&ok); if (!ok) { response = "ERROR: Invalid w"; return; }
            graph.addEdge(u, v, w);
            response = "OK: Edge added " + QString::number(u) + "-" + QString::number(v) + " W=" + QString::number(w, 'f', 1);
        } else if (parts[0] == "SHORTEST_PATH" && parts.size() == 2) {
            bool ok;
            int src = parts[1].toInt(&ok); if (!ok) { response = "ERROR: Invalid src"; return; }
            response = graph.shortestPath(src);
        } else if (parts[0] == "SAVE_DB" && parts.size() == 2) {
            QString filename = parts[1];
            response = db.saveToFile(filename, graph);
        } else if (parts[0] == "LOAD_DB" && parts.size() == 2) {
            QString filename = parts[1];
            response = db.loadFromFile(filename, graph);
        } else if (parts[0] == "STORE_KEY" && parts.size() == 2) {
            QString halfText = parts[1];
            QString encrypted = vigenereCipher(halfText, true);
            std::cout << "STORE_KEY: '" << halfText.toStdString() << "' encrypted to '" << encrypted.toStdString() << "'" << std::endl;
            storedKey = hashWithSha512(encrypted);
            std::cout << "Stored hash: " << storedKey.toStdString() << std::endl;
            response = "OK: Key stored";
        } else if (parts[0] == "GET_KEY" && parts.size() == 2) {
            QString halfText = parts[1];
            QString encrypted = vigenereCipher(halfText, true);
            std::cout << "GET_KEY: '" << halfText.toStdString() << "' encrypted to '" << encrypted.toStdString() << "'" << std::endl;
            QString computedHash = hashWithSha512(encrypted);
            std::cout << "Computed hash: " << computedHash.toStdString() << ", Stored hash: " << storedKey.toStdString() << std::endl;
            if (computedHash == storedKey) {
                QString decrypted = vigenereCipher(encrypted, false);
                std::cout << "Decrypted '" << encrypted.toStdString() << "' to '" << decrypted.toStdString() << "'" << std::endl;
                response = "RESULT: " + decrypted;
            } else {
                response = "ERROR: Key not found";
            }
        } else if (parts[0] == "HALF" && parts.size() == 5) {
            bool ok;
            double a = parts[1].toDouble(&ok);
            if (!ok) {
                response = "ERROR: Invalid a";
            } else {
                double b = parts[2].toDouble(&ok);
                if (!ok) {
                    response = "ERROR: Invalid b";
                } else {
                    double epsilon = parts[3].toDouble(&ok);
                    if (!ok) {
                        response = "ERROR: Invalid epsilon";
                    } else {
                        QString expr = parts[4];
                        response = halfMethod(a, b, epsilon, expr);
                    }
                }
            }
        } else if (parts[0] == "PRINT_DATA") {
            response = graph.printData();
        } else if (parts[0] == "QUIT") {
            response = "BYE";
            std::cout << "Server response: " << response.toStdString() << std::endl;
            QDataStream out(clientSocket);
            out.setVersion(QDataStream::Qt_5_15);
            out << response;
            clientSocket->flush();
            clientSocket->close();
            authenticatedUsers.remove(clientSocket);
            return;
        } else {
            response = "ERROR: Unknown command";
        }

        std::cout << "Server response: " << response.toStdString() << std::endl;
        QDataStream out(clientSocket);
        out.setVersion(QDataStream::Qt_5_15);
        out << response;
        clientSocket->flush();
        std::cout << "Response sent, bytes written: " << clientSocket->bytesToWrite() << std::endl;
    });

    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    std::cout << "Disconnected slot connected for client" << std::endl;
}

bool Server::isAuthenticated(QTcpSocket* socket) {
    return !authenticatedUsers[socket].isEmpty();
}

void Server::saveUser(const QString& login, const QString& password) {
    QFile file(usersFile);
    if (file.open(QIODevice::Append)) {
        QTextStream out(&file);
        out << login << ":" << password << "\n";
        file.close();
    }
}

bool Server::checkUser(const QString& login, const QString& password) {
    QFile file(usersFile);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(":");
            if (parts.size() == 2 && parts[0] == login && parts[1] == password) {
                file.close();
                return true;
            }
        }
        file.close();
    }
    return false;
}
