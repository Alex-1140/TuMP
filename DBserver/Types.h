#ifndef TYPES_H
#define TYPES_H

#include <unordered_map>
#include <vector>
#include <utility>

using Vertex = int;
using Weight = double;
using AdjList = std::unordered_map<Vertex, std::vector<std::pair<Vertex, Weight>>>;

#endif // TYPES_H
