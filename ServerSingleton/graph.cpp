#include "graph.h"
#include <QSet>
#include <QQueue>
#include <limits>

Graph::Graph() {}

void Graph::addEdge(int u, int v, double w) {
    adjList[u][v] = w;
    adjList[v][u] = w; // Ненаправленный граф
}

QString Graph::shortestPath(int src) {
    QMap<int, double> distances;
    for (auto u : adjList.keys()) {
        distances[u] = std::numeric_limits<double>::infinity();
    }
    distances[src] = 0;

    QSet<int> visited;
    QQueue<int> queue;
    queue.enqueue(src);
    visited.insert(src);

    while (!queue.isEmpty()) {
        int u = queue.dequeue();
        for (auto v : adjList[u].keys()) {
            if (!visited.contains(v)) {
                double w = adjList[u][v];
                if (distances[u] + w < distances[v]) {
                    distances[v] = distances[u] + w;
                }
                visited.insert(v);
                queue.enqueue(v);
            }
        }
    }

    QString result = "DISTANCES from " + QString::number(src) + ":\n";
    for (auto v : distances.keys()) {
        result += "[" + QString::number(v) + ":" + QString::number(distances[v], 'f', 1) + "] ";
    }
    return result;
}

QString Graph::printData() {
    QString result = "STRING C ДАННЫМИ\n";
    for (auto u : adjList.keys()) {
        for (auto v : adjList[u].keys()) {
            result += QString::number(u) + " " + QString::number(v) + " " + QString::number(adjList[u][v], 'f', 1) + "\n";
        }
    }
    return result;
}

const QMap<int, QMap<int, double>>& Graph::getAdjList() const {
    return adjList;
}