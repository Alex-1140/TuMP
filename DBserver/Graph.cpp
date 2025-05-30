#include "Graph.h"
#include <queue>
#include <limits>
#include <sstream>

void Graph::addEdge(Vertex u, Vertex v, Weight w) {
    std::lock_guard<std::mutex> lock(mutex_);
    graph_[u].emplace_back(v, w);
    graph_[v].emplace_back(u, w); // граф неориентированный
}

std::vector<Weight> Graph::shortestPath(Vertex src) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto INF = std::numeric_limits<Weight>::infinity();

    Vertex max_v = 0;
    for (auto& kv : graph_) if (kv.first > max_v) max_v = kv.first;

    std::vector<Weight> dist(max_v + 1, INF);
    dist[src] = 0;

    std::priority_queue<std::pair<Weight, Vertex>, std::vector<std::pair<Weight, Vertex>>, std::greater<>> pq;
    pq.emplace(0, src);

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (auto& [v, w] : graph_[u]) {
            if (dist[v] > d + w) {
                dist[v] = d + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}

void Graph::saveGraph(std::ostream& os) const {
    std::lock_guard<std::mutex> lock(mutex_);
    os << "[GRAPH]\n";
    for (auto& [u, edges] : graph_) {
        for (auto& [v, w] : edges) {
            if (u < v)
                os << u << ' ' << v << ' ' << w << '\n';
        }
    }
}

void Graph::loadGraph(std::ifstream& ifs) {
    std::lock_guard<std::mutex> lock(mutex_);
    graph_.clear();
    std::string line;
    while (std::getline(ifs, line) && line != "[VIGENERE]") {
        if (!line.empty() && line != "[GRAPH]") {
            Vertex u, v; Weight w;
            std::istringstream iss(line);
            iss >> u >> v >> w;
            graph_[u].emplace_back(v, w);
            graph_[v].emplace_back(u, w);
        }
    }
}
