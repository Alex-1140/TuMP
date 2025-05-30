#ifndef GRAPH_H
#define GRAPH_H

#include "Types.h"
#include <fstream>
#include <mutex>
#include <vector>

class Graph {
public:
    Graph() = default;
    ~Graph() = default;

    void addEdge(Vertex u, Vertex v, Weight w);
    std::vector<Weight> shortestPath(Vertex src);
    void saveGraph(std::ostream& os) const;
    void loadGraph(std::ifstream& ifs);

private:
    AdjList graph_;
    mutable std::mutex mutex_; // Made mutable to allow locking in const methods
};

#endif // GRAPH_H
