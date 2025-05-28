#pragma once
#include <QString>
#include <QMap>
#include <QVector>
#include <QPair>

class GraphProcessor {
public:
    GraphProcessor();

    int findShortestPath(const QString& from, const QString& to);
    QString vigenereEncrypt(const QString& text, const QString& key);

private:
    QMap<QString, QVector<QPair<QString, int>>> graph;

    void initGraph();
};
