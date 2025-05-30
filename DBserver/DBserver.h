#ifndef DBSERVER_H
#define DBSERVER_H

#include "Graph.h"
#include "Vigenere.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <string>
#include <iostream>

class DBServer : public QTcpServer {
    Q_OBJECT
public:
    DBServer(quint16 port, QObject* parent = nullptr);
    ~DBServer() override = default;

    void addEdge(Vertex u, Vertex v, Weight w);
    std::vector<Weight> shortestPath(Vertex src);
    bool saveDatabase(const std::string& filename);
    bool loadDatabase(const std::string& filename);
    void storeHalfKey(const std::string& half_key, const std::string& text, const std::string& result);
    bool getHalfKey(const std::string& half_key, const std::string& text, std::string& out);
    void printData(std::ostream& os = std::cout) const;

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void readClientData();
    void clientDisconnected();

private:
    Graph graph_;
    Vigenere vigenere_;
    std::map<qintptr, QTcpSocket*> clients_; // Track connected clients
};

#endif // DBSERVER_H
