#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include "tetris.h"

struct GraphNode {
    Tetrimino tetrimino;
    bool visited = false;
    GraphNode *prev;
    std::vector<GraphNode*> neighbours;
};

typedef std::vector<std::vector<GraphNode>> Graph; // first dimension is row second dimension is column

void setNodeNeighbours(GraphNode& node, Graph& graph, Tetrimino& tetrimino, GameGrid& grid);
Graph makeGraph(int width, int height, Tetrimino& tetrimino, GameGrid& grid);
std::vector<GraphNode*> search(Graph &graph, Tetrimino& tetrimino, GameGrid& grid);

#endif