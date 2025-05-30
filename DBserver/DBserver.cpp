#include "DBServer.h"
#include <QDataStream>
#include <sstream>
#include <fstream>

DBServer::DBServer(quint16 port, QObject* parent) : QTcpServer(parent) {
    if (!listen(QHostAddress::Any, port)) {
        std::cerr << "Server could not start on port " << port << ": " << qPrintable(errorString()) << std::endl;
    } else {
        std::cout << "Server started on port " << port << std::endl;
    }
}

void DBServer::addEdge(Vertex u, Vertex v, Weight w) {
    graph_.addEdge(u, v, w);
}

std::vector<Weight> DBServer::shortestPath(Vertex src) {
    return graph_.shortestPath(src);
}

bool DBServer::saveDatabase(const std::string& filename) {
    std::ofstream ofs(filename);
    if (!ofs) return false;
    graph_.saveGraph(ofs);
    vigenere_.saveVigenere(ofs);
    return true;
}

bool DBServer::loadDatabase(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs) return false;
    graph_.loadGraph(ifs);
    vigenere_.loadVigenere(ifs);
    return true;
}

void DBServer::storeHalfKey(const std::string& half_key, const std::string& text, const std::string& result) {
    vigenere_.storeHalfKey(half_key, text, result);
}

bool DBServer::getHalfKey(const std::string& half_key, const std::string& text, std::string& out) {
    return vigenere_.getHalfKey(half_key, text, out);
}

void DBServer::printData(std::ostream& os) const {
    os << "[GRAPH]\n";
    graph_.saveGraph(os);
    os << "[VIGENERE]\n";
    vigenere_.saveVigenere(os);
}

void DBServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket* client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);
    clients_[socketDescriptor] = client;

    connect(client, &QTcpSocket::readyRead, this, &DBServer::readClientData);
    connect(client, &QTcpSocket::disconnected, this, &DBServer::clientDisconnected);

    std::cout << "New client connected: " << socketDescriptor << std::endl;
}

void DBServer::readClientData() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QDataStream in(client);
    in.setVersion(QDataStream::Qt_6_0);

    QString data;
    in >> data;
    if (data.isEmpty()) return;

    std::string command = data.toStdString();
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    std::ostringstream response;
    if (cmd == "ADD_EDGE") {
        Vertex u, v; Weight w;
        if (iss >> u >> v >> w) {
            addEdge(u, v, w);
            response << "Edge added: " << u << " - " << v << " weight: " << w << "\n";
        } else {
            response << "Error: Invalid ADD_EDGE format\n";
        }
    } else if (cmd == "SHORTEST_PATH") {
        Vertex src;
        if (iss >> src) {
            auto dist = shortestPath(src);
            response << "Distances from " << src << ":";
            for (size_t i = 0; i < dist.size(); ++i) {
                if (dist[i] < std::numeric_limits<Weight>::infinity()) {
                    response << " [" << i << ": " << dist[i] << "]";
                }
            }
            response << "\n";
        } else {
            response << "Error: Invalid SHORTEST_PATH format\n";
        }
    } else if (cmd == "STORE_KEY") {
        std::string half_key, text, result;
        if (iss >> half_key >> text >> result) {
            storeHalfKey(half_key, text, result);
            response << "Stored key: " << half_key << " text: " << text << " result: " << result << "\n";
        } else {
            response << "Error: Invalid STORE_KEY format\n";
        }
    } else if (cmd == "GET_KEY") {
        std::string half_key, text, out;
        if (iss >> half_key >> text) {
            if (getHalfKey(half_key, text, out)) {
                response << "Found result: " << out << "\n";
            } else {
                response << "No result found for key: " << half_key << " text: " << text << "\n";
            }
        } else {
            response << "Error: Invalid GET_KEY format\n";
        }
    } else if (cmd == "SAVE_DB") {
        std::string filename;
        if (iss >> filename) {
            if (saveDatabase(filename)) {
                response << "Database saved to " << filename << "\n";
            } else {
                response << "Error: Failed to save database to " << filename << "\n";
            }
        } else {
            response << "Error: Invalid SAVE_DB format\n";
        }
    } else if (cmd == "LOAD_DB") {
        std::string filename;
        if (iss >> filename) {
            if (loadDatabase(filename)) {
                response << "Database loaded from " << filename << "\n";
            } else {
                response << "Error: Failed to load database from " << filename << "\n";
            }
        } else {
            response << "Error: Invalid LOAD_DB format\n";
        }
    } else if (cmd == "PRINT_DATA") {
        printData(response);
    } else {
        response << "Error: Unknown command " << cmd << "\n";
    }

    client->write(QString::fromStdString(response.str()).toUtf8());
    client->flush();
}

void DBServer::clientDisconnected() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    qintptr descriptor = client->socketDescriptor();
    clients_.erase(descriptor);
    client->deleteLater();
    std::cout << "Client disconnected: " << descriptor << std::endl;
}
