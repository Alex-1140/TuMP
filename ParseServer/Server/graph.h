#ifndef GRAPH_H
#define GRAPH_H

#include <QMap>
#include <QSet>
#include <QQueue>
#include <QString>
#include <limits>

class Graph {
public:
    Graph();
    void addEdge(int u, int v, double w);
    QString shortestPath(int src);
    QString printData();
    const QMap<int, QMap<int, double>>& getAdjList() const; // Геттер для adjList

private:
    QMap<int, QMap<int, double>> adjList;
};

#endif // GRAPH_H
