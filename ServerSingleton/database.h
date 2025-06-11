#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include "graph.h"

class Database {
public:
    Database();
    QString saveToFile(const QString& filename, const Graph& graph);
    QString loadFromFile(const QString& filename, Graph& graph);
};

#endif // DATABASE_H