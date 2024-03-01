#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <queue>
#include <vector>
#include "parser.h"

class Graph {
public:
    int graph_size = 0;
    std::vector<int> end_jobs_id;
    std::vector<std::vector<int>> adjacency;

    Graph(int size) {
        graph_size = size;
        adjacency.resize(size);
    }
    void addNode(int startNode, int endNode) {
        adjacency[startNode].push_back(endNode);
    }

    int getSize() {
        return graph_size;
    }

    int isEmpty() {
        return graph_size == 0;
    }

    void print() { 
        for (int startNode = 1; startNode < graph_size; ++startNode) {
            for (int vertex: adjacency[startNode]) {
                std::cout << startNode << ' ' << vertex << "\n";
            }
        }
    }
};

Graph CreateGraph(std::vector<Configuration>);
std::vector<int> BFS(Graph &, std::vector<int> &);
int IsCycle(int, Graph &, std::vector<int> &);

#endif