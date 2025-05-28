#include "GraphProcessor.h"

GraphProcessor::GraphProcessor() {
    initGraph();
}

void GraphProcessor::initGraph() {
    graph["A"] = {{"B", 5}, {"C", 2}};
    graph["B"] = {{"C", 1}, {"D", 3}};
    graph["C"] = {{"D", 7}};
    graph["D"] = {};
}

int GraphProcessor::findShortestPath(const QString& from, const QString& to) {
    QMap<QString, int> dist;
    for (auto it = graph.begin(); it != graph.end(); ++it) {
        dist[it.key()] = INT_MAX;
    }
    dist[from] = 0;

    QVector<QString> queue = {from};
    while (!queue.isEmpty()) {
        QString current = queue.takeFirst();
        int currentDist = dist[current];
        for (const auto& neighbor : graph[current]) {
            if (dist[neighbor.first] > currentDist + neighbor.second) {
                dist[neighbor.first] = currentDist + neighbor.second;
                queue.append(neighbor.first);
            }
        }
    }
    return dist[to];
}

QString GraphProcessor::vigenereEncrypt(const QString& text, const QString& key) {
    QString result;
    int keyLen = key.length();
    for (int i = 0; i < text.length(); ++i) {
        QChar t = text[i];
        QChar k = key[i % keyLen];
        if (t.isLetter()) {
            char base = t.isUpper() ? 'A' : 'a';
            QChar encryptedChar = QChar(((t.toLatin1() - base + k.toLower().toLatin1() - 'a') % 26) + base);
            result.append(encryptedChar);
        } else {
            result.append(t);
        }
    }
    return result;
}
