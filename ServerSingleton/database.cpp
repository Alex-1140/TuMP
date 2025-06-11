#include "database.h"
#include <QFile>
#include <QTextStream>
#include <QIODevice>

Database::Database() {}

QString Database::saveToFile(const QString& filename, const Graph& graph) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return "ERROR: ...";
    }
    QTextStream out(&file);
    const QMap<int, QMap<int, double>>& adjList = graph.getAdjList(); // Используем геттер
    for (auto u : adjList.keys()) {
        for (auto v : adjList[u].keys()) {
            out << u << " " << v << " " << adjList[u][v] << "\n";
        }
    }
    file.close();
    return "OK: Database saved";
}

QString Database::loadFromFile(const QString& filename, Graph& graph) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return "ERROR: ...";
    }
    graph = Graph(); // Очищаем граф
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(" ");
        if (parts.size() == 3) {
            int u = parts[0].toInt();
            int v = parts[1].toInt();
            double w = parts[2].toDouble();
            graph.addEdge(u, v, w);
        }
    }
    file.close();
    return "OK: Database loaded";
}