#include "server.h"
#include <QDataStream>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <cmath>
#include <QDebug>
#include <iostream>

Server::Server(QObject* parent) : QObject(parent) {
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::handleNewConnection);

    if (!tcpServer->listen(QHostAddress::Any, 12345)) {
        qCritical() << "Server failed to start:" << tcpServer->errorString();
        exit(1);
    }
    qInfo() << "Server listening on port 12345";
}

void Server::handleNewConnection() {
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    authenticatedUsers[clientSocket] = "";

    connect(clientSocket, &QTcpSocket::readyRead, this, [=]() {
        QDataStream in(clientSocket);
        in.setVersion(QDataStream::Qt_6_0);

        QString command;
        in >> command;
        QStringList parts = command.split(" ");
        QString response;

        if (parts[0] == "REGISTER" && parts.size() == 3) {
            QString login = parts[1];
            QString password = hashWithSha512(parts[2]);
            if (!UserDatabase::getInstance().addUser(login, password)) {
                response = "ERROR: Could not register user";
            }
            else {
                response = "OK: Registered";
            }
        }
        else if (parts[0] == "LOGIN" && parts.size() == 3) {
            QString login = parts[1];
            QString password = hashWithSha512(parts[2]);
            if (UserDatabase::getInstance().checkUser(login, password)) {
                authenticatedUsers[clientSocket] = login;
                response = "OK: Login successful";
            }
            else {
                response = "ERROR: Invalid login/password";
            }
        }
        else if (!isAuthenticated(clientSocket)) {
            response = "ERROR: Please login first";
        }
        else if (parts[0] == "ADD_EDGE" && parts.size() == 4) {
            bool ok;
            int u = parts[1].toInt(&ok);
            int v = parts[2].toInt(&ok);
            double w = parts[3].toDouble(&ok);
            if (!ok) {
                response = "ERROR: Invalid parameters";
            }
            else {
                graph.addEdge(u, v, w);
                response = "OK: Edge added";
            }
        }
        else if (parts[0] == "SHORTEST_PATH" && parts.size() == 2) {
            int src = parts[1].toInt();
            response = graph.shortestPath(src);
        }
        else if (parts[0] == "SAVE_DB" && parts.size() == 2) {
            response = db.saveToFile(parts[1], graph);
        }
        else if (parts[0] == "LOAD_DB" && parts.size() == 2) {
            response = db.loadFromFile(parts[1], graph);
        }
        else if (parts[0] == "PRINT_DATA") {
            response = graph.printData();
        }
        else if (parts[0] == "HALF" && parts.size() == 5) {
            bool ok;
            double a = parts[1].toDouble(&ok);
            if (!ok) {
                response = "ERROR: Invalid a";
            }
            else {
                double b = parts[2].toDouble(&ok);
                if (!ok) {
                    response = "ERROR: Invalid b";
                }
                else {
                    double epsilon = parts[3].toDouble(&ok);
                    if (!ok) {
                        response = "ERROR: Invalid epsilon";
                    }
                    else {
                        QString expr = parts[4];
                        response = halfMethod(a, b, epsilon, expr);
                    }
                }
            }
        }
        else if (parts[0] == "STORE_KEY" && parts.size() == 2) {
            QString halfText = parts[1];
            QString encrypted = vigenereCipher(halfText, true);
            std::cout << "STORE_KEY: '" << halfText.toStdString() << "' encrypted to '" << encrypted.toStdString() << "'" << std::endl;
            storedKey = hashWithSha512(encrypted);
            std::cout << "Stored hash: " << storedKey.toStdString() << std::endl;
            response = "OK: Key stored";
        }
        else if (parts[0] == "GET_KEY" && parts.size() == 2) {
            QString halfText = parts[1];
            QString encrypted = vigenereCipher(halfText, true);
            std::cout << "GET_KEY: '" << halfText.toStdString() << "' encrypted to '" << encrypted.toStdString() << "'" << std::endl;
            QString computedHash = hashWithSha512(encrypted);
            std::cout << "Computed hash: " << computedHash.toStdString() << ", Stored hash: " << storedKey.toStdString() << std::endl;
            if (computedHash == storedKey) {
                QString decrypted = vigenereCipher(encrypted, false);
                std::cout << "Decrypted '" << encrypted.toStdString() << "' to '" << decrypted.toStdString() << "'" << std::endl;
                response = "RESULT: " + decrypted;
            }
            else {
                response = "ERROR: Key mismatch";
            }
        }
        else {
            response = "ERROR: Unknown command";
        }

        QDataStream out(clientSocket);
        out.setVersion(QDataStream::Qt_6_0);
        out << response;
        clientSocket->flush();
        });

    connect(clientSocket, &QTcpSocket::disconnected, this, [=]() {
        authenticatedUsers.remove(clientSocket);
        clientSocket->deleteLater();
        });
}

bool Server::isAuthenticated(QTcpSocket* socket) {
    return authenticatedUsers.contains(socket) && !authenticatedUsers[socket].isEmpty();
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
        }
        else {
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
        }
        else {
            a = c;
            fa = fc;
        }
    }
    return "RESULT: Root ≈ " + QString::number(c, 'f', 6);
}